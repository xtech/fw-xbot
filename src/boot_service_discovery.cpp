//
// Created by clemens on 01.10.24.
//

#include "boot_service_discovery.hpp"

// clang-format off
#include "ch.h"
#include "hal.h"
// clang-format on

#include <stdlib.h>
#include <string.h>

#include "chprintf.h"
#include "lwip/sockets.h"
static char boardAdvertisementBuffer[100];
static char boardAdvertisementRequestBuffer[100];
static THD_WORKING_AREA(waServiceDiscovery, 512);

static void multicast_sender_thread(void *p) {
  (void)p;
  struct sockaddr_in multicast_addr;
  struct sockaddr_in addr;
  int sockfd;
  struct timeval tv;

  // Create a new socket
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    return;
  }

  // Set up the local address and port
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Any local address
  addr.sin_port = htons(SD_MULTICAST_PORT);

  // Bind the socket to the specified address and port
  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    close(sockfd);
    return;
  }

  int broadcast = 1;

  if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
    close(sockfd);
    return;
  }

  // Set the receive timeout
  tv.tv_sec = SD_INTERVAL / 1000;
  tv.tv_usec = (SD_INTERVAL % 1000) * 1000;
  if (lwip_setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    close(sockfd);
    return;
  }

  // Setup the multicast address structure for sending the advertisement
  memset(&multicast_addr, 0, sizeof(multicast_addr));
  multicast_addr.sin_family = AF_INET;
  multicast_addr.sin_addr.s_addr = inet_addr(SD_MULTICAST_GROUP);
  multicast_addr.sin_port = htons(SD_MULTICAST_PORT);
  while (true) {
    int received =
        recvfrom(sockfd, boardAdvertisementRequestBuffer, sizeof(boardAdvertisementRequestBuffer) - 1, 0, NULL, NULL);
    if (received > 0) {
      // Make sure, there's a zero terminator
      boardAdvertisementRequestBuffer[received] = 0;
      // check, if the command was "RESET", if so, we reset
      if (strncmp(boardAdvertisementRequestBuffer, "RESET", sizeof(boardAdvertisementRequestBuffer)) == 0) {
        // Set the boot address to bootloader and reset the system
        MODIFY_REG(SYSCFG->UR2, SYSCFG_UR2_BOOT_ADD0, (0x8000000 >> 16) << SYSCFG_UR2_BOOT_ADD0_Pos);
        NVIC_SystemReset();
        while (1)
          ;
      } else if (strncmp(boardAdvertisementRequestBuffer, "DISCOVER_REQUEST",
                         sizeof(boardAdvertisementRequestBuffer)) == 0) {
        // Send the multicast message
        sendto(sockfd, boardAdvertisementBuffer, strlen(boardAdvertisementBuffer), 0,
               (struct sockaddr *)&multicast_addr, sizeof(multicast_addr));
      }
    }
  }

  // Close the socket
  close(sockfd);
}

void InitBootloaderServiceDiscovery() {
  memset(boardAdvertisementBuffer, 0, sizeof(boardAdvertisementBuffer));
  chsnprintf(boardAdvertisementBuffer, sizeof(boardAdvertisementBuffer),
             "BOARD_ADVERTISEMENT:xcore-boot;application-mode");

  // Create a multicast sender thread
  thread_t *threadPointer =
      chThdCreateStatic(waServiceDiscovery, sizeof(waServiceDiscovery), NORMALPRIO, multicast_sender_thread, NULL);
  threadPointer->name = "Boot SD";
}

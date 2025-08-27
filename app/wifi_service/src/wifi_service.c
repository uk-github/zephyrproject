/*
 * Copyright (c) 2024 Muhammad Haziq
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/dhcpv4_server.h>
#include "wifi_service.h"

#define LOG_TAG "WIFI_SVC"

#define MACSTR "%02X:%02X:%02X:%02X:%02X:%02X"

#define NET_EVENT_WIFI_MASK                                                  \
	(NET_EVENT_WIFI_CONNECT_RESULT | NET_EVENT_WIFI_DISCONNECT_RESULT |      \
	 NET_EVENT_WIFI_AP_ENABLE_RESULT | NET_EVENT_WIFI_AP_DISABLE_RESULT |    \
	 NET_EVENT_WIFI_AP_STA_CONNECTED | NET_EVENT_WIFI_AP_STA_DISCONNECTED)

/* AP Mode Configuration */
#define WIFI_AP_SSID       "ESP32-AP"
#define WIFI_AP_PSK        ""
#define WIFI_AP_IP_ADDRESS "192.168.4.1"
#define WIFI_AP_NETMASK    "255.255.255.0"

/* STA Mode Configuration */
#define WIFI_SSID "Disconnet"     											/* Replace `SSID` with WiFi ssid. */
#define WIFI_PSK  "qwerfdsa" 												/* Replace `PASSWORD` with Router password. */

static struct net_if *ap_iface;
static struct net_if *sta_iface;

static struct wifi_connect_req_params ap_config;
static struct wifi_connect_req_params sta_config;

static struct net_mgmt_event_callback cb;

static void wifi_event_handler(struct net_mgmt_event_callback *cb, uint64_t mgmt_event,
			       struct net_if *iface) {
    BEGIN();
	switch (mgmt_event) {
        case NET_EVENT_WIFI_CONNECT_RESULT: {
            log_i("Connected to %s", WIFI_SSID);
        } break;
        case NET_EVENT_WIFI_DISCONNECT_RESULT: {
            log_i("Disconnected from %s", WIFI_SSID);
        } break;
        case NET_EVENT_WIFI_AP_ENABLE_RESULT: {
            log_i("AP Mode is enabled. Waiting for station to connect");
        } break;
        case NET_EVENT_WIFI_AP_DISABLE_RESULT: {
            log_i("AP Mode is disabled.");
        } break;
        case NET_EVENT_WIFI_AP_STA_CONNECTED: {
            struct wifi_ap_sta_info *sta_info = (struct wifi_ap_sta_info *)cb->info;

            log_i("station: " MACSTR " joined ", sta_info->mac[0], sta_info->mac[1],
                sta_info->mac[2], sta_info->mac[3], sta_info->mac[4], sta_info->mac[5]);
        } break;
        case NET_EVENT_WIFI_AP_STA_DISCONNECTED: {
            struct wifi_ap_sta_info *sta_info = (struct wifi_ap_sta_info *)cb->info;

            log_i("station: " MACSTR " leave ", sta_info->mac[0], sta_info->mac[1],
                sta_info->mac[2], sta_info->mac[3], sta_info->mac[4], sta_info->mac[5]);
        } break;
        default:
            break;
	}
    END();
}

static void enable_dhcpv4_server(void) {
    BEGIN();
	static struct in_addr addr;
	static struct in_addr netmaskAddr;

	if (net_addr_pton(AF_INET, WIFI_AP_IP_ADDRESS, &addr)) {
		log_e("Invalid address: %s", WIFI_AP_IP_ADDRESS);
		return;
	}

	if (net_addr_pton(AF_INET, WIFI_AP_NETMASK, &netmaskAddr)) {
		log_e("Invalid netmask: %s", WIFI_AP_NETMASK);
		return;
	}

	net_if_ipv4_set_gw(ap_iface, &addr);

	if (net_if_ipv4_addr_add(ap_iface, &addr, NET_ADDR_MANUAL, 0) == NULL) {
		log_e("unable to set IP address for AP interface");
	}

	if (!net_if_ipv4_set_netmask_by_addr(ap_iface, &addr, &netmaskAddr)) {
		log_e("Unable to set netmask for AP interface: %s", WIFI_AP_NETMASK);
	}

	addr.s4_addr[3] += 10; 														/* Starting IPv4 address for DHCPv4 address pool. */

	if (net_dhcpv4_server_start(ap_iface, &addr) != 0) {
		log_e("DHCP server is not started for desired IP");
		return;
	}

	log_i("DHCPv4 server started...\n");
    END();
}

static int enable_ap_mode(void) {
    BEGIN();
	if (!ap_iface) {
		log_i("AP: is not initialized");
		return -EIO;
	}

	log_i("Turning on AP Mode");
	ap_config.ssid 			= (const uint8_t *)WIFI_AP_SSID;
	ap_config.ssid_length 	= strlen(WIFI_AP_SSID);
	ap_config.psk 			= (const uint8_t *)WIFI_AP_PSK;
	ap_config.psk_length 	= strlen(WIFI_AP_PSK);
	ap_config.channel 		= WIFI_CHANNEL_ANY;
	ap_config.band 			= WIFI_FREQ_BAND_2_4_GHZ;

	if (strlen(WIFI_AP_PSK) == 0) {
		ap_config.security = WIFI_SECURITY_TYPE_NONE;
	} else {

		ap_config.security = WIFI_SECURITY_TYPE_PSK;
	}

	enable_dhcpv4_server();

	int ret = net_mgmt(NET_REQUEST_WIFI_AP_ENABLE, ap_iface, &ap_config,
			   sizeof(struct wifi_connect_req_params));
	if (ret) {
		log_e("NET_REQUEST_WIFI_AP_ENABLE failed, err: %d", ret);
	}
    END();
	return ret;
}

static int connect_to_wifi(void) {
	if (!sta_iface) {
		log_i("STA: interface no initialized");
		return -EIO;
	}

	sta_config.ssid 		= (const uint8_t *)WIFI_SSID;
	sta_config.ssid_length 	= strlen(WIFI_SSID);
	sta_config.psk 			= (const uint8_t *)WIFI_PSK;
	sta_config.psk_length 	= strlen(WIFI_PSK);
	sta_config.security 	= WIFI_SECURITY_TYPE_PSK;
	sta_config.channel 		= WIFI_CHANNEL_ANY;
	sta_config.band 		= WIFI_FREQ_BAND_2_4_GHZ;

	log_i("Connecting to SSID: %s\n", sta_config.ssid);

	int ret = net_mgmt(NET_REQUEST_WIFI_CONNECT, sta_iface, &sta_config,
			   sizeof(struct wifi_connect_req_params));
	if (ret) {
		log_e("Unable to Connect to (%s)", WIFI_SSID);
	}

	return ret;
}

int wifi_service_start(void) {
    BEGIN();
	k_sleep(K_SECONDS(5));

	net_mgmt_init_event_callback(&cb, wifi_event_handler, NET_EVENT_WIFI_MASK);
	net_mgmt_add_event_callback(&cb);

	ap_iface = net_if_get_wifi_sap();															/* Get AP interface in AP-STA mode. */
	sta_iface = net_if_get_wifi_sta();															/* Get STA interface in AP-STA mode. */

	enable_ap_mode();
	connect_to_wifi();
    END();
	return 0;
}

int wifi_service_init(void) {
    BEGIN();
    END();
	return 0;
}

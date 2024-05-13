#include <Preferences.h>
#include "nvs_flash.h"

#ifndef memoryManager_H
#define memoryManager_H

/// @brief Class responsible for all read/write operations
class memoryManager{
    private:
        Preferences pref_; // Preferences instance

    public:
        /// @brief Checks if wifi details are setup
        /// @return Boolean representing wifi state
        bool isSetup();

        /// @brief Writes wifi details into memory
        /// @param ID SSID
        /// @param PASS Password
        /// @return Boolean if writing was successful
        bool writeNetwork(const char * ID, const char * PASS);
        
        /// @brief Writes default radio link
        /// @param Link Link of radio channel
        /// @return Boolean if writing was successful
        bool writeDefault(const char * Link);

        /// @brief Disables wifi flag in memory
        /// @return Operation status
        bool disableWiFi();

        /// @brief Resets all settings
        void resetConfig();

        /// @brief Resets all audio settings
        /// @return Operation status
        bool resetAudio();

        /// @brief Gets default radio link
        /// @return Default radio link
        String getDefault();

        /// @brief Gets SSID 
        /// @return SSID
        String getSSID();

        /// @brief Gets stored password
        /// @return Password
        String getPass();
};

#endif
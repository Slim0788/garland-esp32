#include <Preferences.h>

class Prefs
{
private:
#define PREF_NAME "pref_name"
#define KEY_SSID "arg_ssid"
#define KEY_SSID_DEF ""
#define KEY_PASS "arg_password"
#define KEY_PASS_DEF ""
#define KEY_MODE "arg_last_mode"
#define KEY_MODE_DEF 0

    static Preferences preferences;

public:
    static void setWiFiCredentials(String ssid, String password);
    static String getSsid();
    static String getPassword();

    static void setLastMode(int modeCode);
    static int getLastMode();
};

// std::string
void Prefs::setWiFiCredentials(String ssid, String password)
{
    preferences.begin(PREF_NAME, false);
    preferences.putString(KEY_SSID, ssid);
    preferences.putString(KEY_PASS, password);
    Serial.printf("Network credentials have been saved:\nSSID: %s\nPASSWORD: %s\n", ssid, password);
    preferences.end();
}

String Prefs::getSsid()
{
    preferences.begin(PREF_NAME, true);
    String ssid = preferences.getString(KEY_SSID, KEY_SSID_DEF);
    preferences.end();
    return ssid;
}

String Prefs::getPassword()
{
    preferences.begin(PREF_NAME, true);
    String password = preferences.getString(KEY_PASS, KEY_PASS_DEF);
    preferences.end();
    return password;
}

void Prefs::setLastMode(int modeCode)
{
    preferences.begin(PREF_NAME, false);
    preferences.putInt(KEY_MODE, modeCode);
    Serial.printf("Last mode number is: %d\n", modeCode);
    preferences.end();
}

int Prefs::getLastMode()
{
    preferences.begin(PREF_NAME, true);
    int lastMode = preferences.getInt(KEY_MODE, KEY_MODE_DEF);
    preferences.end();
    return lastMode;
}
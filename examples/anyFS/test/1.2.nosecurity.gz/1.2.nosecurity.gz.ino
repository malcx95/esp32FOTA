/**
   esp32 firmware OTA

   Purpose: Perform an OTA update to both firmware and filesystem from binaries located
            on a webserver (HTTPS) without checking for certificate validity

   Usage: If the ESP32 had a previous successful WiFi connection, then no need to set the ssid/password
          to run this sketch, the credentials are still cached :-)
          Sketch 1 will FOTA to Sketch 2, then Sketch 3, and so on until all versions in firmware.json are
          exhausted.


*/

#include <flashz.hpp> // optional esp32-flashz for gzipped firmwares
#include <esp32FOTA.hpp>
#include <debug/test_fota_common.h>

// esp32fota settings
int firmware_version_major  = 1;
int firmware_version_minor  = 2;
int firmware_version_patch  = 0;

//  #define FOTA_URL "http://server/fota/fota.json"

const char* firmware_name   = "esp32-fota-http";
const bool check_signature  = false;
const bool disable_security = true;
// for debug only
const char* title           = "1.2";
const char* description     = "Basic *gzipped* example with no security and no filesystem";

const char* fota_debug_fmt = R"DBG_FMT(

***************** STAGE %s *****************

  Description      : %s
  Firmware type    : %s
  Firmware version : %i.%i.%i
  Signature check  : %s
  TLS Cert check   : %s
  Compression      : %s

********************************************

)DBG_FMT";


// esp32fota esp32fota("<Type of Firmware for this device>", <this version>, <validate signature>, <allow insecure TLS>);
// esp32FOTA esp32FOTA( String(firmware_name), firmware_version, check_signature, disable_security );


esp32FOTA FOTA;


void setup_wifi()
{
  delay(10);

  Serial.print("MAC Address ");
  Serial.println( WiFi.macAddress() );

  WiFi.begin(); // no WiFi creds in this demo :-)

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println(WiFi.localIP());
}


void setup()
{
  Serial.begin(115200);

  PrintFOTAInfo();

  {
    auto cfg = FOTA.getConfig();
    cfg.name         = firmware_name;
    cfg.manifest_url = FOTA_URL;
    cfg.sem          = SemverClass( firmware_version_major, firmware_version_minor, firmware_version_patch );
    cfg.check_sig    = check_signature;
    cfg.unsafe       = disable_security;
    //cfg.root_ca      = MyRootCA;
    //cfg.pub_key      = MyRSAKey;
    FOTA.setConfig( cfg );
  }

  setup_wifi();
}

void loop()
{
  bool updatedNeeded = FOTA.execHTTPcheck();
  if (updatedNeeded) {
    FOTA.execOTA();
  }

  delay(20000);
}


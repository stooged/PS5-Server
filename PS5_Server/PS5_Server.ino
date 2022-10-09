#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>
#include <DNSServer.h>
#include <FS.h>
#include <SPI.h>

//basic esp8266 server for the ps5 exploit files from https://github.com/Cryptogenic/PS5-4.03-Kernel-Exploit

DNSServer dnsServer;
ESP8266WebServer webServer(80);
ESP8266WebServerSecure sWebServer(443);


static const char serverCert[] = "-----BEGIN CERTIFICATE-----\r\nMIIC1DCCAj2gAwIBAgIUFQgjEtkNYfmrrpNQKHVNl3+dl08wDQYJKoZIhvcNAQEL\r\nBQAwfDELMAkGA1UEBhMCVVMxEzARBgNVBAgMCkNhbGlmb3JuaWExEDAOBgNVBAcM\r\nB0ZyZW1vbnQxDDAKBgNVBAoMA2VzcDEMMAoGA1UECwwDZXNwMQwwCgYDVQQDDANl\r\nc3AxHDAaBgkqhkiG9w0BCQEWDWVzcEBlc3AubG9jYWwwHhcNMjEwMjIxMDAwMDQ4\r\nWhcNNDMwNzI4MDAwMDQ4WjB8MQswCQYDVQQGEwJVUzETMBEGA1UECAwKQ2FsaWZv\r\ncm5pYTEQMA4GA1UEBwwHRnJlbW9udDEMMAoGA1UECgwDZXNwMQwwCgYDVQQLDANl\r\nc3AxDDAKBgNVBAMMA2VzcDEcMBoGCSqGSIb3DQEJARYNZXNwQGVzcC5sb2NhbDCB\r\nnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAsrfFqlV5H0ajdAkkZ51HTOseOjYj\r\nNiaUD4MA5mIRonnph6EKIWb9Yl85vVa6yfVkGn3TFebQ96MMdTfZgLuP4ryCwe6Y\r\n+tZs2g6TjGbR0O6yuA8wQ2Ln7E0T05C8oOl88SGNV4tVL6hz64oMzuVebVDo0J9I\r\nybvL0O/LhMvC4x8CAwEAAaNTMFEwHQYDVR0OBBYEFCMQIU+pZQDVySXejfbIYbLQ\r\ncLXiMB8GA1UdIwQYMBaAFCMQIU+pZQDVySXejfbIYbLQcLXiMA8GA1UdEwEB/wQF\r\nMAMBAf8wDQYJKoZIhvcNAQELBQADgYEAFHPz3YhhXQYiERTGzt8r0LhNWdggr7t0\r\nWEVuAoEukjzv+3DVB2O+56NtDa++566gTXBGGar0pWfCwfWCEu5K6MBkBdm6Ub/A\r\nXDy+sRQTqH/jTFFh5lgxeq246kHWHGRad8664V5PoIh+OSa0G3CEB+BXy7WF82Qq\r\nqx0X6E/mDUU=\r\n-----END CERTIFICATE-----";
static const char serverKey[] = "-----BEGIN PRIVATE KEY-----\r\nMIICdgIBADANBgkqhkiG9w0BAQEFAASCAmAwggJcAgEAAoGBALK3xapVeR9Go3QJ\r\nJGedR0zrHjo2IzYmlA+DAOZiEaJ56YehCiFm/WJfOb1Wusn1ZBp90xXm0PejDHU3\r\n2YC7j+K8gsHumPrWbNoOk4xm0dDusrgPMENi5+xNE9OQvKDpfPEhjVeLVS+oc+uK\r\nDM7lXm1Q6NCfSMm7y9Dvy4TLwuMfAgMBAAECgYEApKFbSeyQtfnpSlO9oGEmtDmG\r\nT9NdHl3tWFiydId0fTpWoKT9YwWvdnYIB12klbQicbDkyTEl4Gjnafd3ufmNsaH8\r\nZ9twopIdvvWDvGPIqGNjvTYcuczpXmQWiUnG5OTiVWI1XuZa3uZEGSFK9Ra6bE4g\r\nG2xklGZGdaqqcd6AVhECQQDnBXVXwBxExxSFppL8KUtWgyXAvJAEvkzvTOQfcCel\r\naIM5EEUofB7WZeMtDEKgBtoBl+i5PP+GnDF0zsjDFx2nAkEAxgqVQii6zURSVE2T\r\niJDihySXJ2bmLJUjRIi1nCs64I9Oz4fECVvGwZ1XU8Uzhh3ylyBSG2HjhzA5sTSC\r\n1a/tyQJAOgE12EWFE4PE1FXhm+ymXN9q8DyoEHjTilYNBRO88JwQLpi2NJcNixlj\r\n8+CbLeDqhfHlXfVB10OKa2CsKce5CwJAbhaN+DQJ+3dCSOjC3YSk2Dkn6VhTFW9m\r\nJn/UbNa/KPug9M5k1Er3RsO/OqsBxEk7hHUMD3qv74OIXpBxNnZQuQJASlwk5HZT\r\n7rULkr72fK/YYxkS0czBDIpTKqwklxU+xLSGWkSHvSvl7sK4TmQ1w8KVpjKlTCW9\r\nxKbbW0zVmGN6wQ==\r\n-----END PRIVATE KEY-----";

String defaultIndex = "<!DOCTYPE html><html><head><style>body { background-color: #1451AE;color: #ffffff;font-size: 14px; font-weight: bold; margin: 0 0 0 0.0; padding: 0.4em 0.4em 0.4em 0.6em;}</style></head><center><br><br><br><br><br><br>ESP</center></html>";

String AP_SSID = "PS5_WEB_AP";
String AP_PASS = "password";

IPAddress Server_IP(10,1,1,1);
IPAddress Subnet_Mask(255,255,255,0);


String getContentType(String filename){
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}


void handleHtml() {

    if (loadFromSpiffs(webServer.uri())) {
    return;
    }
  

  String message = "\n\n";
  message += "URI: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";
  for (uint8_t i = 0; i < webServer.args(); i++) {
    message += " NAME:" + webServer.argName(i) + "\n VALUE:" + webServer.arg(i) + "\n";
  }
  webServer.send(404, "text/plain", "Not Found");
  Serial.print(message);
}


bool loadFromSpiffs(String path) {

  if (path.equals("/")) {
    path += "index.html";
  }

  if (path.endsWith("index.html") && !SPIFFS.exists(path)) {
    webServer.setContentLength(defaultIndex.length());
    webServer.send(200, "text/html", defaultIndex);
    return true;
  }

  if (SPIFFS.exists(path)){
    String dataType = getContentType(path);
    File dataFile = SPIFFS.open(path, "r");
  if (!dataFile) {
    return false;
  }

  if (webServer.streamFile(dataFile, dataType) != dataFile.size()) {
    Serial.println("Sent less data than expected!");
  }
    dataFile.close();
    return true;
  }else{
    return false;
  }
}





void setup(void) 
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("SSID: " + AP_SSID);
  Serial.println("Password: " + AP_PASS);
  Serial.println("WEB Server IP: " + Server_IP.toString());
  SPIFFS.begin();
  Serial.println("No SD card, using SPIFFS");
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(Server_IP, Server_IP, Subnet_Mask);
  WiFi.softAP(AP_SSID.c_str(), AP_PASS.c_str());
  Serial.println("WIFI AP started");


  dnsServer.setTTL(30);
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  dnsServer.start(53, "*", Server_IP);
  Serial.println("DNS server started");


  webServer.onNotFound(handleHtml);

  sWebServer.getServer().setRSACert(new X509List(serverCert), new PrivateKey(serverKey));
  sWebServer.onNotFound([]() {
  sWebServer.sendHeader("Location", String("http://" + Server_IP.toString() + "/index.html" ), true);
  sWebServer.send(301, "text/plain", "");
  });


  sWebServer.begin();
  webServer.begin();
  Serial.println("HTTP server started");

}


void loop(void) {
  dnsServer.processNextRequest();
  webServer.handleClient();
  sWebServer.handleClient();
}

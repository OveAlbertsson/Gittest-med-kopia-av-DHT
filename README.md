# DHT_ESP32-C3
Program för att läsa av DHT-givare med ESP32 C3.
Givartyp (11, 22, 21) kan väljas på rad 17 i main. 
Givaren ansluts till GPIO2.
Värden hämtas via webben med ip-nr/getH respektive getT (eller med curl)

För anslutning till den egna routern måste man lägga in egna credentials i main (raderna 11-14, som är bortkommenderade) eller lägga till en egen fil WifiCredential.h enligt nedan under src. Fördelen med det senare är att den filen är undantagen för push/pull till Github genom markering i gitignore.

WifiCredential.h

#define STASSID "SSID"

#define STAPSK "password"
1

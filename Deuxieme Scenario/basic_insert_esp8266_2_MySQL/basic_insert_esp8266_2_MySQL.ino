#include <ESP8266WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

IPAddress server_addr(127, 0, 0, 1);
char user[] = "root";
char password[] = "";

char INSERT_SQL[] = "INSERT INTO temp_humi_dht (id, sensor, location, value1, value2) VALUES (12865261, 'sensor1', '1256', '32', '15')";

char ssid[] = "Pytool";
char pass[] = "11111111";

WiFiClient client;
MySQL_Connection conn(&client);
MySQL_Cursor *cursor;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    delay(100);
  }

  Serial.printf("\nConnecting to %s", ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to network");
  Serial.print("My IP address is: ");
  Serial.println(WiFi.localIP());

  Serial.print("Connecting to SQL...  ");
  if (conn.connect(server_addr, 3306, user, password))
    Serial.println("OK.");
  else
    Serial.println("FAILED.");

  cursor = new MySQL_Cursor(&conn);
}

void loop()
{
  if (conn.connected())
    cursor->execute(INSERT_SQL);

  delay(5000);
}

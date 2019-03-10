#include "webserver.h"

void WebServer2::begin(Preferences *preferences){
  server = WiFiServer(80);
  server.begin();
  this->preferences = preferences;
}
 
void WebServer2::blocking_run(void){
		while (true){
			WiFiClient client = server.available();   // Listen for incoming clients
		  	if (client) {                          // If a new client connects,
		    Serial.println("New Client.");          // print a message out in the serial port
		    String currentLine = "";                // make a String to hold incoming data from the client
		    while (client.connected()) {            // loop while the client's connected
		      if (client.available()) {             // if there's bytes to read from the client,
		        char c = client.read();             // read a byte, then
		        Serial.write(c);                    // print it out the serial monitor
		        header += c;
		        if (c == '\n') {                    // if the byte is a newline character
		          // if the current line is blank, you got two newline characters in a row.
		          // that's the end of the client HTTP request, so send a response:
		          if (currentLine.length() == 0) {
		            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
		            // and a content-type so the client knows what's coming, then a blank line:
		            client.println("HTTP/1.1 200 OK");
		            client.println("Content-type:text/html");
		            client.println("Connection: close");
		            client.println();
		            Serial.println(header);
		            // turns the GPIOs on and off
		            if (header.indexOf("GET /td1") >= 0) {
		              Serial.println("td1 selected");
		              salle="TD1";
		              preferences->putString("salle", salle);
		              delay(200); ESP.restart();
		            } else if (header.indexOf("GET /td2") >= 0) {
		              Serial.println("td2 selected");
		              salle="TD2";
		              preferences->putString("salle", salle);
		              delay(200); ESP.restart();
		            } else if (header.indexOf("GET /td3") >= 0) {
		              Serial.println("td3 selected");
		              salle="TD3";
		              preferences->putString("salle", salle);
		              delay(200); ESP.restart();
		            } else if (header.indexOf("GET /td4") >= 0) {
		              Serial.println("td4 selected");
		              salle="TD4";
		              preferences->putString("salle", salle);
		              delay(200); ESP.restart();
		            } else if (header.indexOf("GET /td5") >= 0) {
		              Serial.println("td5 selected");
		              salle="TD5";
		              preferences->putString("salle", salle);
		              delay(200); ESP.restart();
		            } else if (header.indexOf("GET /td6") >= 0) {
		              Serial.println("td6 selected");
		              salle="TD6";
		              preferences->putString("salle", salle);
		              delay(200); ESP.restart();
		            } else if (header.indexOf("GET /td7") >= 0) {
		              Serial.println("td7 selected");
		              salle="TD7";
		              preferences->putString("salle", salle);
		              delay(200); ESP.restart();
		            } else if (header.indexOf("GET /reseau1") >= 0) {
		              Serial.println("réseau 1 selected");
		              salle="Salle Réseaux 1";
		              preferences->putString("salle", salle);
		              delay(200); ESP.restart();
		            } else if (header.indexOf("GET /reseau2") >= 0) {
		              Serial.println("réseau 2 selected");
		              salle="Salle Réseaux 2";
		              preferences->putString("salle", salle);
		              delay(200); ESP.restart();
		            } else if (header.indexOf("GET /reseau3") >= 0) {
		              Serial.println("réseau 3 selected");
		              salle="Salle Réseaux 3";
		              preferences->putString("salle", salle);
		            } else if (header.indexOf("GET /info1") >= 0) {
		              Serial.println("Salle info 1 selected");
		              salle="Salle Info. 1";
		              preferences->putString("salle", salle);
		              delay(200); ESP.restart();
		            } else if (header.indexOf("GET /info2") >= 0) {
		              Serial.println("Salle info 2 selected");
		              salle="Salle Info. 2";
		              preferences->putString("salle", salle);
		              delay(200); ESP.restart();
		            } else if (header.indexOf("GET /elec1") >= 0) {
		              Serial.println("Elec 1 selected");
		              salle="Salle Elec. 1";
		              preferences->putString("salle", salle);
		              delay(200); ESP.restart();
		            } else if (header.indexOf("GET /text") >= 0) {
		              
		            }
               salle2 = preferences->getString("salle","0");
                Serial.println(salle2);
		            // Display the HTML web page
		            client.println("<!DOCTYPE html><html>");
		            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
		            client.println("<link rel=\"icon\" href=\"data:,\">");
		            client.println("<meta charset=\"utf-8\">");
		            // CSS to style the on/off buttons 
		            // Feel free to change the background-color and font-size attributes to fit your preferences
		            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
		            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
		            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;} </style>");
		            Serial.println();
		            // Web Page Heading
		            client.println("<body><h1>MyConnectedRoom</h1>");
                client.println("<h1>Salle sélectionnée : "+preferences->getString("salle","aucune")+"</h1>");
		            client.println("<p><a onclick=\"return confirm('Reboot for TD1 ?')\" href=\"/td1\"><button class=\"button\">TD1</button></a></p>");
		            client.println("<p><a onclick=\"return confirm('Reboot for TD2 ?')\"href=\"/td2\"><button class=\"button\">TD2</button></a></p>");
		            client.println("  <p><a onclick=\"return confirm('Reboot for TD3 ?')\" href=\"/td3\"><button class=\"button\">TD3</button></a></p>");
		            client.println("  <p><a onclick=\"return confirm('Reboot for TD4 ?')\" href=\"/td4\"><button class=\"button\">TD4</button></a></p>");
		            client.println("  <p><a onclick=\"return confirm('Reboot for TD5 ?')\" href=\"/td5\"><button class=\"button\">TD5</button></a></p>");
		            client.println("  <p><a onclick=\"return confirm('Reboot for TD6 ?')\" href=\"/td6\"><button class=\"button\">TD6</button></a></p>");
		            client.println("  <p><a onclick=\"return confirm('Reboot for TD7 ?')\" href=\"/td7\"><button class=\"button\">TD7</button></a></p>");
		            client.println("  <p><a onclick=\"return confirm('Reboot for Salle Info 1 ?')\" href=\"/info1\"><button class=\"button\">Salle info 1</button></a></p>");
		            client.println("  <p><a onclick=\"return confirm('Reboot for Salle Info 2 ?')\" href=\"/info2\"><button class=\"button\">Salle info 2</button></a></p>");
		            client.println("  <p><a onclick=\"return confirm('Reboot for Salle Réseau 1 ?')\" href=\"/reseau1\"><button class=\"button\">Réseau 1</button></a></p>");
		            client.println("  <p><a onclick=\"return confirm('Reboot for Salle Réseau 2 ?')\" href=\"/reseau2\"><button class=\"button\">Réseau 2</button></a></p>");
		            client.println("  <p><a onclick=\"return confirm('Reboot for Salle Réseau 3 ?')\" href=\"/reseau3\"><button class=\"button\">Réseau 3</button></a></p>");
		            client.println("  <p><a onclick=\"return confirm('Reboot for Télécom 1?')\" href=\"/telecom1\"><button class=\"button\">Télécom 1</button></a></p>");
		            client.println("  <p><a onclick=\"return confirm('Reboot for Télécom 2 ?')\" href=\"/telecom2\"><button class=\"button\">Télécom 2</button></a></p>");
		            client.println("  <p><a onclick=\"return confirm('Reboot for Elec 1?')\" href=\"/elec1\"><button class=\"button\">Élec 1</button></a></p>");
		            client.println("  <input type=\"text\" name=\"sethu\" id=\"hu\" value=\"Enter code\" />");
		            client.println("  <input type=\"button\" value=\"Send message\" onclick=\"getHU();\" />");
		            client.println("</body>");
		            client.println("<script type=\"text/javascript\">");
		            client.println("  function getHU(){");
		            client.println("       var hu = document.getElementById(\"hu\").value;");
		            client.println("       const req = new XMLHttpRequest();");
		            client.println("       req.open(\"GET\", \"/text/\"+hu, false);");
		            client.println("       req.send(null);}");
		            client.println("</script>");
		            client.println("</body></html>");
		            
		            // The HTTP response ends with another blank line
		            client.println();
		            // Break out of the while loop
		            break;
		          } else { // if you got a newline, then clear currentLine
		            currentLine = "";
		          }
		        } else if (c != '\r') {  // if you got anything else but a carriage return character,
		          currentLine += c;      // add it to the end of the currentLine
		        }
		      }
		    }
		    // Clear the header variable
		    header = "";
		    // Close the connection
		    client.stop();
		    Serial.println("Client disconnected.");
		    Serial.println("");
		  }
	}
}

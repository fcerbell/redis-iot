MENUFCT(twitteruser) {
  Serial.println();
  Serial.println("Please enter your Twitter account name");
  Serial.print("[");
  Serial.print(getCommandByName("twitteruser")->value);
  Serial.println("] >");
  CurrentState = twitteruser_answer;
}

void twitteruser_answer() {
  if (true == getAnswer(getCommandByName("twitteruser")->value))
    CurrentState = menu_func;
}

MENUFCT(twittertest) {
  Serial.println();
  if (WiFi.status() != WL_CONNECTED)
    wifitest_func();
  Serial.print("Testing Twitter account [");
  Serial.print(getCommandByName("twitteruser")->value);
  Serial.print("]...");
  String url = (String)"https://twitter.com/"
               + getCommandByName("twitteruser")->value;
  HTTPClient http;
  http.begin(url, "68 2D 7F F1 B1 3E 09 5B F5 DA AA 63 2E CE 51 F4 DF 5B B1 55");
  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    Serial.print("Error (");
    Serial.print(httpCode);
    Serial.println(")");
    //    CurrentState = twitteruser_func;
    CurrentState = menu_func;
    http.end();
    return;
  }
  Serial.println("OK");

  Serial.print("Testing Twitter keywords...");
  kw_parse(http);
  http.end();
  kw_show();
  kg_show();

  if ((groups[0] < 20) || (groups[3] < 50)) {
    Serial.println("");
    Serial.println("Please post a tweet about the meetup with the hashtags.");
    delay(10000);
    //    CurrentState = twitteruser_func;
    CurrentState = menu_func;
    return;
  }

  Serial.println("");
  CurrentState = menu_func;
  return;
}


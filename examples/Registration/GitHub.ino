MENUFCT(githubuser) {
  Serial.println();
  Serial.println("Please enter your GitHub account name");
  Serial.print("[");
  Serial.print(getCommandByName("githubuser")->value);
  Serial.println("] >");
  CurrentState = githubuser_answer;
}

void githubuser_answer() {
  if (true == getAnswer(getCommandByName("githubuser")->value))
    CurrentState = menu_func;
}

MENUFCT(githubproject) {
  Serial.println();
  Serial.println("Please enter the name of your GitHub Redis-IoT project");
  Serial.print("[");
  Serial.print(getCommandByName("githubproject")->value);
  Serial.println("] >");
  CurrentState = githubproject_answer;
}

void githubproject_answer() {
  if (true == getAnswer(getCommandByName("githubproject")->value))
    CurrentState = menu_func;
}

MENUFCT(githubtest) {
  HTTPClient http;
  String url;
  int httpCode;
  Serial.println();
  if (WiFi.status() != WL_CONNECTED)
    wifitest_func();
  Serial.print("Testing GitHub account [");
  Serial.print(getCommandByName("githubuser")->value);
  Serial.print("]... ");
  url = (String)"https://github.com/"
        + getCommandByName("githubuser")->value;
  http.begin(url, "D7 9F 07 61 10 B3 92 93 E3 49 AC 89 84 5B 03 80 C1 9E 2F 8B");
  httpCode = http.GET();
  http.end();
  if (httpCode != HTTP_CODE_OK) {
    Serial.print("Error non existing GitHub account");
    Serial.println(httpCode);
    //    CurrentState = githubuser_func;
    CurrentState = menu_func;
    return;
  }
  Serial.println("OK...");

  Serial.print("Testing Github project [");
  Serial.print(getCommandByName("githubproject")->value);
  Serial.print("]... ");
  url = (String)"https://github.com/"
        + getCommandByName("githubuser")->value
        + "/"
        + getCommandByName("githubproject")->value;
  http.begin(url, "D7 9F 07 61 10 B3 92 93 E3 49 AC 89 84 5B 03 80 C1 9E 2F 8B");
  httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    Serial.print("Error non existing project (");
    Serial.print(httpCode);
    Serial.println(")");
    http.end();
    //    CurrentState = githubuser_func;
    CurrentState = menu_func;
    return;
  }
  Serial.println("OK...");

  Serial.print("Parsing Github project keywords...");
  kw_parse(http);
  http.end();
  kw_show();
  kg_show();

  if ((groups[0] < 20) || (groups[3] < 50)) {
    Serial.println("");
    Serial.println("Please use the keywords in your project title and description");
    Serial.println("it helps me to find sponsor for fun meetups such as this one.");
    delay(10000);
    //    CurrentState = githubuser_func;
    CurrentState = menu_func;
    return;
  }

  Serial.println("");
  CurrentState = menu_func;
  return;
}

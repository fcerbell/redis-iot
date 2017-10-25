MENUFCT(redishost) {
  Serial.println();
  Serial.println("Please enter the redis server hostname or IP address");
  Serial.print("[");
  Serial.print(getCommandByName("redishost")->value);
  Serial.println("] >");
  CurrentState = redishost_answer;
}

void redishost_answer() {
  if (true == getAnswer(getCommandByName("redishost")->value))
    CurrentState = menu_func;
}

MENUFCT(redispass) {
  Serial.println();
  Serial.println("Please enter the redis db password");
  Serial.print("[");
  Serial.print(getCommandByName("redispass")->value);
  Serial.println("] >");
  CurrentState = redispass_answer;
}

void redispass_answer() {
  if (true == getAnswer(getCommandByName("redispass")->value))
    CurrentState = menu_func;
}

MENUFCT(redisport) {
  Serial.println();
  Serial.println("Please enter the redis server port number");
  Serial.print("[");
  Serial.print(getCommandByName("redisport")->value);
  Serial.println("] >");
  free(tmpAnswer); tmpAnswer = strdup(getCommandByName("redisport")->value);
  CurrentState = redisport_answer;
}

void redisport_answer() {
  if (true == getAnswer(tmpAnswer)) {
    static int choice = -1;
    choice = atoi(tmpAnswer);
    if ((choice <= 0) || (choice > 65535)) {
      Serial.println("Please enter a valid port number");
      free(tmpAnswer); tmpAnswer = strdup(getCommandByName("redisport")->value);
      CurrentState = redisport_func;
      return;
    }
    free(getCommandByName("redisport")->value);
    getCommandByName("redisport")->value = strdup(tmpAnswer);
    CurrentState = menu_func;
  }
}

MENUFCT(redistest) {
  Serial.println();
  if (WiFi.status() != WL_CONNECTED)
    wifitest_func();
  Serial.print("Testing Redis endpoint DNS resolution [");
  Serial.print(getCommandByName("redishost")->value);
  Serial.print("]...");
  IPAddress redisEndpointIP;
  WiFi.hostByName(getCommandByName("redishost")->value, redisEndpointIP);
  Serial.print("IP:");
  Serial.println(redisEndpointIP);

  Serial.print("Testing Redis connection [");
  Serial.print(redisEndpointIP);
  Serial.print(":");
  Serial.print(getCommandByName("redisport")->value);
  Serial.print("]...");
  WiFiClient redis;
  if (!redis.connect(redisEndpointIP, atoi(getCommandByName("redisport")->value))) {
    Serial.println(" Failed");
    CurrentState = redishost_func;
    return;
  }
  Serial.println("OK...");
  Serial.print("Testing Redis authentication (AUTH)...");
  redis.print((String("*2\r\n$4\r\nAUTH\r\n$3\r\n") + getCommandByName("redispass")->value + "\r\n").c_str());
  delay(100);
  while (redis.available() != 0)
    Serial.print((char)redis.read());

  Serial.print("Testing Redis health (INFO)...");
  redis.print("*1\r\n$4\r\nINFO\r\n");
  delay(100);
  if (!redis.find("redis_version:")) {
    Serial.println("No redis_version answer to INFO");
    CurrentState = redishost_func;
    return;
  }
  Serial.println("OK");
  CurrentState = menu_func;
}


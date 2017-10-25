uint8_t nbMenuChoices = 0;

MENUFCT(menu) {
  Serial.println();
  Serial.println("Redis IoT Meetup - Registration");
  Serial.println("2017 F.Cerbelle");
  Serial.println("v1.0");
  Serial.println();
  Serial.println("This tool looks like an AS400. It could use a webserver, but you would");
  Serial.println("have to connect to this interface, at least once to know the webserver URL to");
  Serial.println("connect to !!!! Furthermore, it would add complexity to the tool, with");
  Serial.println("potential bugs and no added value at all. So, KISS !");
  Serial.println();
  Serial.println("Did you create your GitHub project and a twit before ? You should ! ;)");
  Serial.println();
  Serial.println("Menu:");
  nbMenuChoices = 0;
  for (Command_t *it = commands; * (char*)it != 0; it++) {
    nbMenuChoices++;
    Serial.print((it - commands));
    Serial.print(" - ");
    Serial.print(it->label);
    if ((it->showValue) && (it->value[0] != 0)) {
      Serial.print(" [");
      Serial.print(it->value);
      Serial.print("]");
    }
    Serial.println();
  }
  Serial.println();
  Serial.println("Please enter your choice >");

  free(tmpAnswer); tmpAnswer = strdup("0");
  CurrentState = menu_answer;
}

void menu_answer() {
  if (true == getAnswer(tmpAnswer)) {
    static int choice = -1;
    choice = atoi(tmpAnswer);
    if ((choice < 0) || (choice >= nbMenuChoices)) {
      Serial.print("Bad choice : "); Serial.println(choice);
      Serial.print("Same player shoot again... (between 0 and "); Serial.print(nbMenuChoices - 1); Serial.println(")");
      free(tmpAnswer); tmpAnswer = strdup("0");
      CurrentState = menu_func;
      return;
    }
    CurrentState = commands[choice].func;
  }
}



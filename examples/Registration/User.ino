MENUFCT(firstname) {
  Serial.println();
  Serial.println("Please enter your firstname");
  Serial.print("[");
  Serial.print(getCommandByName("firstname")->value);
  Serial.println("] >");
  CurrentState = firstname_answer;
}

void firstname_answer() {
  if (true == getAnswer(getCommandByName("firstname")->value))
    CurrentState = menu_func;
}

MENUFCT(lastname) {
  Serial.println();
  Serial.println("Please enter your lastname");
  Serial.print("[");
  Serial.print(getCommandByName("lastname")->value);
  Serial.println("] >");
  CurrentState = lastname_answer;
}

void lastname_answer() {
  if (true == getAnswer(getCommandByName("lastname")->value))
    CurrentState = menu_func;
}

MENUFCT(email) {
  Serial.println();
  Serial.println("Please enter your email");
  Serial.println("There is no sanity check, please enter it carefully");
  Serial.print("[");
  Serial.print(getCommandByName("email")->value);
  Serial.println("] >");
  CurrentState = email_answer;
}

void email_answer() {
  if (true == getAnswer(getCommandByName("email")->value))
    CurrentState = menu_func;
}

MENUFCT(screenname) {
  Serial.println();
  Serial.println("Please enter your screenname");
  Serial.println("You can simply use your firstname and lastname");
  Serial.println("Make it original, it has to be unique in the room");
  Serial.print("[");
  Serial.print(getCommandByName("screenname")->value);
  Serial.println("] >");
  CurrentState = screenname_answer;
}

void screenname_answer() {
  if (true == getAnswer(getCommandByName("screenname")->value))
    CurrentState = menu_func;
}


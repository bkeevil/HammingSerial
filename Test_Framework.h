#ifndef Test_Framework_h
#define Test_Framework_h

void printTestTitle(char *s) {
  Serial.println("");
  Serial.println(s);
}

void printTestSubtitle(char *s) {
  Serial.println("");
  Serial.print(" ");
  Serial.println(s);
}

void printTestResult(char *s,bool b) {
  if (b)
    Serial.print("  PASS ");
  else
    Serial.print("  FAIL ");
  Serial.println(s);
}

#endif


void pressStart() {
  SerialGeneric.println("pressStart()");
  pressStartTime = millis() - 1000; // as set in setPressMs()
} // pressStart()


void singleClick() {
  SerialGeneric.println("singleClick() detected.");
  single_press= true;
} // singleClick


void duringPress(){



    SerialGeneric.print("duringPress(");
  SerialGeneric.print(millis() - pressStartTime);
  SerialGeneric.println(") detected.");
 
}


void doubleClick() {
  SerialGeneric.println("doubleClick() detected.");
  long_press=true;

} // doubleClick
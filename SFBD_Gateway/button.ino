
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
esp_task_wdt_reset();
if((millis() - button.getPressedMs()) >=10000){
  reset_setting = true;
  
}
SerialGeneric.print(millis() - pressStartTime);
SerialGeneric.println(") detected.");
 
}


void doubleClick() {
  SerialGeneric.println("doubleClick() detected.");
  reset_setting=true;

} // doubleClick
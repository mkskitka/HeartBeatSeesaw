void testQueue() {
  cppQueue  q1(sizeof(int), 3, IMPLEMENTATION, true); // Instantiate queue
  int v1 = 7;
  int v2 = 8; 
  int v3 = 9; 
  q1.push(&v1);
  q1.push(&v2);
  q1.push(&v3);
  //Serial.println(String(q1.getCount()) + "== 3");
  int ref;
  q1.pop(&ref);
  Serial.println(String(ref) + " popped from queue ");
   q1.pop(&ref);
  Serial.println(String(ref) + " popped from queue ");
  Serial.println(String(q1.getCount()) + "== 1");
}

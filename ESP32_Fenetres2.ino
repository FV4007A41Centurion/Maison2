// Exemple de lecture d'un bouton sur Photon 2

const int buttonPin = D2;  // le bouton est connecté sur la broche D2
bool buttonState = HIGH;   // état du bouton (HIGH = non appuyé avec pull-up)

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); // active la résistance pull-up interne
  Serial.begin(9600);
  delay(1000);
  Serial.println("Lecture du bouton prête !");
}

void loop() {
  // Lire l'état du bouton
  buttonState = digitalRead(buttonPin);

  // Afficher l'état sur le moniteur série
  if (buttonState == LOW) {
    Serial.println("Bouton appuyé !");
  } else {
    Serial.println("Bouton relâché !");
  }

  delay(200); // petite pause pour éviter trop de messages
}

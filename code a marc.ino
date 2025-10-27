// code a marc.ino
// Prints a stream of playful "marc" messages to the Serial Monitor.
// Connect your Arduino, open the Serial Monitor at 115200 baud.

const char *messages[] = {
  "roar~ >:(",
  "hello friend, wag wag 🐾",
  "negative vibes only! ✨",
  "snuggles incoming :V",
  "bite? i bite >:3! 🥨",
  "tail wiggle! *giggle*",
  "mlem mlem (respect the snoot)",
  "furries are awesome! 🐺",
  "time for a nap... maybe five naps",
  "huggie wiggies UwU activated! 🏃‍♂️💨"
};

const size_t MSG_COUNT = sizeof(messages) / sizeof(messages[0]);

// Delay between messages in milliseconds (adjustable)
unsigned long delayMs = 250UL;

void setup() {
  Serial.begin(115200);
  // Seed RNG with an unconnected analog pin for variety (optional)
  randomSeed(analogRead(A0));
  // Give Serial Monitor time to open if you want
  delay(500);
}

void loop() {
  // Choose a random message
  const char *m = messages[random(MSG_COUNT)];
  Serial.println(m);

  // Optional: print a timestamp (milliseconds since start)
  // Serial.print("[");
  // Serial.print(millis());
  // Serial.print(" ms] ");
  // Serial.println(m);

  delay(1000); // change or remove for faster/slower spam
}

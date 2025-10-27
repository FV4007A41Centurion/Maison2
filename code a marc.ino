// furry_spam.ino
// Prints a stream of playful "furry" messages to the Serial Monitor.
// Connect your Arduino, open the Serial Monitor at 115200 baud.

const char *messages[] = {
  "rawr~ >:3",
  "hello friend, wag wag 🐾",
  "pawsitive vibes only! ✨",
  "snuggles incoming :3",
  "got snacks? i has snacks! 🥨",
  "tail wiggle! *wiggle*",
  "mlem mlem (respect the snoot)",
  "fursuits are awesome! 🐺",
  "time for a nap... maybe five naps",
  "zoomies activated! 🏃‍♂️💨"
};

const size_t MSG_COUNT = sizeof(messages) / sizeof(messages[0]);

// Delay between messages in milliseconds (adjustable)
unsigned long delayMs = 250UL;

void setup() {
  Serial.begin(115200);
  // Seed RNG with an unconnected analog pin for variety (optional)
  randomSeed(analogRead(A0));
  // Give Serial Monitor time to open if you want
  delay(200);
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

  delay(delayMs); // change or remove for faster/slower spam
}

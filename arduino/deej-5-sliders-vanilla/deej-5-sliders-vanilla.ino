
#include <Simple5641AS.h>
// segmentPins[] = { a, b, c, d, e, f, g, dot };
const uint8_t segmentPins[] = { 12, 8, 4, 6, 7, 11, 3, 5};
// digitSelectionPins[] = { D1, D2, D3, D4 };
const uint8_t digitSelectionPins[] = { 13, 10, 9, 2 };
Simple5641AS component( segmentPins, digitSelectionPins );

const int NUM_SLIDERS = 5;
const int analogInputs[NUM_SLIDERS] = {A0, A1, A2, A3, A4};

int analogSliderValues[NUM_SLIDERS];
int oldSliderValues[NUM_SLIDERS] = {0, 0, 0, 0, 0};
int activeSlider = -1;  // -1 signifie qu'aucun slider n'est actif
int old_percentage = 0;  // Pour éviter les sauts de valeurs sur l'affichage

const float MOVEMENT_THRESHOLD = 10;  // Seuil de mouvement pour considérer un slider comme actif (en pourcentage)
const long INACTIVITY_TIMEOUT = 5000;  // Éteindre l'écran après 5 secondes d'inactivité
long lastMovementTime = 0;

void setup() { 
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(analogInputs[i], INPUT);
  }

  Serial.begin(9600);
}

void loop() {
  updateSliderValues();
  sendSliderValues(); // Envoyer les données en série
  updateDisplay();

  delay(10);
}

void updateSliderValues() {
  bool movement = false;

  for (int i = 0; i < NUM_SLIDERS; i++) {
    analogSliderValues[i] = analogRead(analogInputs[i]);
    
    // Vérifier si ce slider a bougé significativement
    if (abs(analogSliderValues[i] - oldSliderValues[i]) > MOVEMENT_THRESHOLD) {
      activeSlider = i;  // Ce slider devient le slider actif
      lastMovementTime = millis();
      movement = true;
    }
    
    oldSliderValues[i] = analogSliderValues[i];
  }
}

void updateDisplay() {
  // Vérifier si nous avons dépassé le délai d'inactivité
  if (millis() - lastMovementTime > INACTIVITY_TIMEOUT) {
    component.clean();  // Éteindre l'écran
    return;
  }
  
  // Afficher la valeur du slider actif s'il y en a un
  if (activeSlider >= 0) {
    int percentage = map(analogSliderValues[activeSlider], 0, 1020, 0, 100);
    
    // Ne mettre à jour l'affichage que si la valeur a changé significativement
    // ou si un certain temps s'est écoulé
    if (abs(old_percentage - percentage) > 1 || millis() - lastMovementTime < 500) {
      component.clean();
      component.displayNumber_nocycles(percentage);
      old_percentage = percentage;
    } else {
      component.clean();
      component.displayNumber_nocycles(old_percentage);
    }
  }
}

void sendSliderValues() {
  String builtString = String("");

  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += String((int)analogSliderValues[i]);

    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }

  Serial.println(builtString);
}

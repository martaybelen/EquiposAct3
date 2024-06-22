//Funcion para la configuración de la lógica difusa
void configuracionIluminacionFuzzy(){

  //DEFINE EL CONJUNTO DIFUSO PARA LA INTENSIDAD DE LA LUZ
  FuzzyInput *lightInput = new FuzzyInput(1);
  //Definición de los Conjuntos Difusos para la Entrada
  FuzzySet *oscuro = new FuzzySet(0, 0, 200, 400);
  FuzzySet *medio = new FuzzySet(200, 400, 600, 800);
  FuzzySet *iluminado = new FuzzySet(600, 800, 1000, 1000);
  lightInput->addFuzzySet(oscuro);
  lightInput->addFuzzySet(medio);
  lightInput->addFuzzySet(iluminado);
  fuzzy->addFuzzyInput(lightInput);

 //configuración logica difusa para leds
 //DEFINE EL CONJUNTO DIFUSO PARA LOS LEDS
  FuzzyOutput *ledOutput = new FuzzyOutput(1);
  //CONJUNTO DIFUSO
  FuzzySet *fewLeds = new FuzzySet(0, 0, 2, 4);
  FuzzySet *someLeds = new FuzzySet(2, 4, 6, 8);
  FuzzySet *manyLeds = new FuzzySet(6, 8, 8, 8);
  ledOutput->addFuzzySet(fewLeds);
  ledOutput->addFuzzySet(someLeds);
  ledOutput->addFuzzySet(manyLeds);
  fuzzy->addFuzzyOutput(ledOutput);

//Regla 1: "If oscuro, Then Few LEDs"
  FuzzyRuleAntecedent *ifoscuro = new FuzzyRuleAntecedent();
  ifoscuro->joinSingle(oscuro);
  FuzzyRuleConsequent *thenFewLeds = new FuzzyRuleConsequent();
  thenFewLeds->addOutput(fewLeds);
  //se crea la regla 1
  FuzzyRule *rule1 = new FuzzyRule(1, ifoscuro, thenFewLeds);
  //se añade la regla 1 al sistema
  fuzzy->addFuzzyRule(rule1);

//Regla 2: "If medio, Then Some LEDs"
  FuzzyRuleAntecedent *ifmedio = new FuzzyRuleAntecedent();
  ifmedio->joinSingle(medio);
  FuzzyRuleConsequent *thenSomeLeds = new FuzzyRuleConsequent();
  thenSomeLeds->addOutput(someLeds);
  //Creación de la regla difusa 2
  FuzzyRule *rule2 = new FuzzyRule(2, ifmedio, thenSomeLeds);
  //se añade la relga 2
  fuzzy->addFuzzyRule(rule2);

//Regla 3: "If iluminado, Then Many LEDs"
  FuzzyRuleAntecedent *ifiluminado = new FuzzyRuleAntecedent();
  ifiluminado->joinSingle(iluminado);
  FuzzyRuleConsequent *thenManyLeds = new FuzzyRuleConsequent();
  thenManyLeds->addOutput(manyLeds);
  //Creacion regla difusa 3
  FuzzyRule *rule3 = new FuzzyRule(3, ifiluminado, thenManyLeds);
  //se añade la regla 3
  fuzzy->addFuzzyRule(rule3);
}
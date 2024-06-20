void configuracionTyHFuzzy() {
  // Configuración de conjuntos difusos para temperatura
  FuzzyInput *temperature = new FuzzyInput(1);
  FuzzySet *cold = new FuzzySet(-40, -40, 0, 10);
  FuzzySet *comfortable = new FuzzySet(20, 25, 25, 30);
  FuzzySet *hot = new FuzzySet(25, 30, 40, 40);
  temperature->addFuzzySet(cold);
  temperature->addFuzzySet(comfortable);
  temperature->addFuzzySet(hot);
  fuzzy->addFuzzyInput(temperature);

  // Configuración de conjuntos difusos para humedad
  FuzzyInput *humidity = new FuzzyInput(2);
  FuzzySet *dry = new FuzzySet(0, 0, 30, 40);
  FuzzySet *normal = new FuzzySet(30, 40, 60, 70);
  FuzzySet *humid = new FuzzySet(60, 70, 100, 100);
  humidity->addFuzzySet(dry);
  humidity->addFuzzySet(normal);
  humidity->addFuzzySet(humid);
  fuzzy->addFuzzyInput(humidity);

  // Configuración de conjunto difuso para nivel de confort
  FuzzyOutput *comfort = new FuzzyOutput(1);
  FuzzySet *uncomfortable = new FuzzySet(0, 0, 25, 50);
  FuzzySet *comfortable_output = new FuzzySet(50, 75, 100, 100);
  comfort->addFuzzySet(uncomfortable);
  comfort->addFuzzySet(comfortable_output);
  fuzzy->addFuzzyOutput(comfort);

  // Definición de reglas difusas
  FuzzyRuleAntecedent *ifColdAndDry = new FuzzyRuleAntecedent();
  ifColdAndDry->joinWithAND(cold, dry);
  FuzzyRuleConsequent *thenUncomfortable = new FuzzyRuleConsequent();
  thenUncomfortable->addOutput(uncomfortable);
  FuzzyRule *rule1 = new FuzzyRule(1, ifColdAndDry, thenUncomfortable);
  fuzzy->addFuzzyRule(rule1);

  FuzzyRuleAntecedent *ifColdAndNormal = new FuzzyRuleAntecedent();
  ifColdAndNormal->joinWithAND(cold, normal);
  FuzzyRuleConsequent *thenUncomfortable2 = new FuzzyRuleConsequent();
  thenUncomfortable2->addOutput(uncomfortable);
  FuzzyRule *rule2 = new FuzzyRule(2, ifColdAndNormal, thenUncomfortable2);
  fuzzy->addFuzzyRule(rule2);

  FuzzyRuleAntecedent *ifComfortableAndNormal = new FuzzyRuleAntecedent();
  ifComfortableAndNormal->joinWithAND(comfortable, normal);
  FuzzyRuleConsequent *thenComfortable = new FuzzyRuleConsequent();
  thenComfortable->addOutput(comfortable_output);
  FuzzyRule *rule3 = new FuzzyRule(3, ifComfortableAndNormal, thenComfortable);
  fuzzy->addFuzzyRule(rule3);

  FuzzyRuleAntecedent *ifHotAndHumid = new FuzzyRuleAntecedent();
  ifHotAndHumid->joinWithAND(hot, humid);
  FuzzyRuleConsequent *thenUncomfortable3 = new FuzzyRuleConsequent();
  thenUncomfortable3->addOutput(uncomfortable);
  FuzzyRule *rule4 = new FuzzyRule(4, ifHotAndHumid, thenUncomfortable3);
  fuzzy->addFuzzyRule(rule4);
}
void configuracionTyHFuzzy() {
  // Configuración de conjuntos difusos para temperatura
  // Crear conjuntos difusos para la temperatura
  FuzzySet *temp_baja = new FuzzySet(-40, -10, 0, 10);
  FuzzySet *temp_media = new FuzzySet(15, 20, 25, 30);
  FuzzySet *temp_alta = new FuzzySet(25, 40, 80, 80);

  // Crear conjuntos difusos para la humedad
  FuzzySet *humedad_baja = new FuzzySet(0, 0, 30, 40);
  FuzzySet *humedad_media = new FuzzySet(30, 40, 60, 70);
  FuzzySet *humedad_alta = new FuzzySet(60, 70, 100, 100);

  // Crear conjuntos difusos para el control del servomotor
  FuzzySet *servo_cerrado = new FuzzySet(0, 0, 0, 45);
  FuzzySet *servo_medio = new FuzzySet(45, 45, 90, 135);
  FuzzySet *servo_abierto = new FuzzySet(90, 135, 180, 180);

  // Añadir variables de entrada a la lógica difusa
  FuzzyInput *temperatura = new FuzzyInput(1);
  temperatura->addFuzzySet(temp_baja);
  temperatura->addFuzzySet(temp_media);
  temperatura->addFuzzySet(temp_alta);
  fuzzy->addFuzzyInput(temperatura);

  FuzzyInput *humedad = new FuzzyInput(2);
  humedad->addFuzzySet(humedad_baja);
  humedad->addFuzzySet(humedad_media);
  humedad->addFuzzySet(humedad_alta);
  fuzzy->addFuzzyInput(humedad);

  FuzzyOutput *servo = new FuzzyOutput(3);
  servo->addFuzzySet(servo_cerrado);
  servo->addFuzzySet(servo_medio);
  servo->addFuzzySet(servo_abierto);
  fuzzy->addFuzzyOutput(servo);

  // Crear reglas difusas
  // Regla 1: Si la temperatura es baja y la humedad es baja, entonces el servomotor está abierto
  FuzzyRuleAntecedent *if_temp_baja_and_humedad_baja = new FuzzyRuleAntecedent();
  if_temp_baja_and_humedad_baja->joinWithAND(temp_baja, humedad_baja);
  FuzzyRuleConsequent *then_servo_abierto = new FuzzyRuleConsequent();
  then_servo_abierto->addOutput(servo_abierto);
  FuzzyRule *fuzzyRule1 = new FuzzyRule(1, if_temp_baja_and_humedad_baja, then_servo_abierto);
  fuzzy->addFuzzyRule(fuzzyRule1);

  // Regla 2: Si la temperatura es media y la humedad es media, entonces el servomotor está medio abierto
  FuzzyRuleAntecedent *if_temp_media_and_humedad_media = new FuzzyRuleAntecedent();
  if_temp_media_and_humedad_media->joinWithAND(temp_media, humedad_media);
  FuzzyRuleConsequent *then_servo_medio = new FuzzyRuleConsequent();
  then_servo_medio->addOutput(servo_medio);
  FuzzyRule *fuzzyRule2 = new FuzzyRule(2, if_temp_media_and_humedad_media, then_servo_medio);
  fuzzy->addFuzzyRule(fuzzyRule2);

  // Regla 3: Si la temperatura es alta y la humedad es alta, entonces el servomotor está cerrado
  FuzzyRuleAntecedent *if_temp_alta_and_humedad_alta = new FuzzyRuleAntecedent();
  if_temp_alta_and_humedad_alta->joinWithAND(temp_alta, humedad_alta);
  FuzzyRuleConsequent *then_servo_cerrado = new FuzzyRuleConsequent();
  then_servo_cerrado->addOutput(servo_cerrado);
  FuzzyRule *fuzzyRule3 = new FuzzyRule(3, if_temp_alta_and_humedad_alta, then_servo_cerrado);
  fuzzy->addFuzzyRule(fuzzyRule3);

  // Regla 4: Si la temperatura es baja y la humedad es media, entonces el servomotor está medio abierto
  FuzzyRuleAntecedent *if_temp_baja_and_humedad_media = new FuzzyRuleAntecedent();
  if_temp_baja_and_humedad_media->joinWithAND(temp_baja, humedad_media);
  FuzzyRule *fuzzyRule4 = new FuzzyRule(4, if_temp_baja_and_humedad_media, then_servo_medio);
  fuzzy->addFuzzyRule(fuzzyRule4);

  // Regla 5: Si la temperatura es media y la humedad es baja, entonces el servomotor está abierto
  FuzzyRuleAntecedent *if_temp_media_and_humedad_baja = new FuzzyRuleAntecedent();
  if_temp_media_and_humedad_baja->joinWithAND(temp_media, humedad_baja);
  FuzzyRule *fuzzyRule5 = new FuzzyRule(5, if_temp_media_and_humedad_baja, then_servo_abierto);
  fuzzy->addFuzzyRule(fuzzyRule5);

  // Regla 6: Si la temperatura es alta y la humedad es media, entonces el servomotor está cerrado
  FuzzyRuleAntecedent *if_temp_alta_and_humedad_media = new FuzzyRuleAntecedent();
  if_temp_alta_and_humedad_media->joinWithAND(temp_alta, humedad_media);
  FuzzyRule *fuzzyRule6 = new FuzzyRule(6, if_temp_alta_and_humedad_media, then_servo_cerrado);
  fuzzy->addFuzzyRule(fuzzyRule6);

  // Regla 7: Si la temperatura es media y la humedad es alta, entonces el servomotor está medio abierto
  FuzzyRuleAntecedent *if_temp_media_and_humedad_alta = new FuzzyRuleAntecedent();
  if_temp_media_and_humedad_alta->joinWithAND(temp_media, humedad_alta);
  FuzzyRule *fuzzyRule7 = new FuzzyRule(7, if_temp_media_and_humedad_alta, then_servo_medio);
  fuzzy->addFuzzyRule(fuzzyRule7);

  // Regla 8: Si la temperatura es baja y la humedad es alta, entonces el servomotor está cerrado
  FuzzyRuleAntecedent *if_temp_baja_and_humedad_alta = new FuzzyRuleAntecedent();
  if_temp_baja_and_humedad_alta->joinWithAND(temp_baja, humedad_alta);
  FuzzyRule *fuzzyRule8 = new FuzzyRule(8, if_temp_baja_and_humedad_alta, then_servo_cerrado);
  fuzzy->addFuzzyRule(fuzzyRule8);

}
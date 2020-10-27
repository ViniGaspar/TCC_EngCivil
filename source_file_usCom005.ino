

//========================================================================================//
// --- Mapeamento de Hardware ---
#define  dig_uni    8                          //dígito das unidades
#define  dig_dez    9                          //dígito das dezenas
#define  dig_cen   17                          //dígito das centenas
#define  dig_mil   16                          //dígito dos milhares
#define  v_input   A0                          //leitura de tensão
#define  sensor    10                          //sensor no digital 10
#define  bomba     11                          //bomba d'agua


//========================================================================================//
// --- Protótipo das Funções ---
int display_f(int num);                       //Exibe mensagem "FULL" no display
int display_v(int num);                       //Exibe mensagem "VOId" no display



//========================================================================================//
// --- Variávei Globais ---
short control = 1;                             //Variável de controle (para saber qual display está ativo)
int mil, cen, dez, uni;                        //Variáveis auxiliares para dados nos displays
boolean flagAux = 0x00;                        //Flag para determinar se o sensor está acionado


//========================================================================================//
// --- Rotina de Interrupção ---
ISR(TIMER2_OVF_vect)      // Trata Overflow do Timer2
{
    TCNT2 = 196;          // Reinicializa o registrador do Timer2
    
    
    if(!digitalRead(dig_mil) && control == 1)      //Dígito dos milhares desligado?
    {                                              //Variável de controle igual a 1?
        control = 0x02;                            //Sim, control recebe o valor 2
        digitalWrite(dig_uni, LOW);                //Apaga o dígito das unidades
        digitalWrite(dig_dez, LOW);                //Apaga o dígito das dezenas
        digitalWrite(dig_cen, LOW);                //Apaga o dígito das centenas
        PORTD   = 0x00;                            //Desliga PORTD
        mil     = 0;                               //Calcula o dígito dos milhares
        digitalWrite(dig_mil, HIGH);               //Ativa dígito dos milhares
        
        if(flagAux) PORTD   = display_f(mil);                  //Escreve o valor no display dos milhares
        else PORTD = display_v(mil);
        
    } //end if dig_mil
    
    else if(!digitalRead(dig_cen) && control == 2) //Dígito das centenas desligado?
    {                                              //Variável de controle igual a 2?
        control = 0x03;                            //Sim, control recebe o valor 3
        digitalWrite(dig_uni, LOW);                //Apaga o dígito das unidades
        digitalWrite(dig_dez, LOW);                //Apaga o dígito das dezenas
        digitalWrite(dig_mil, LOW);                //Apaga o dígito dos milhares
        PORTD   = 0x00;                            //Desliga PORTD
        cen = 1;                                   //Calcula o dígito das centenas
        digitalWrite(dig_cen, HIGH);               //Ativa dígito das centenas
        
        if(flagAux) PORTD   = display_f(cen);                  //Escreve o valor no display das centenas
        else PORTD = display_v(cen); 
                                                   
    } //end if dig_cen
        
    else if(!digitalRead(dig_dez) && control == 3) //Dígito das dezenas desligado?
    {                                              //Variável de controle igual a 3?
        control = 0x04;                            //Sim, control recebe o valor 3
        digitalWrite(dig_uni, LOW);                //Apaga o dígito das unidades
        digitalWrite(dig_cen, LOW);                //Apaga o dígito das centenas
        digitalWrite(dig_mil, LOW);                //Apaga o dígito dos milhares
        PORTD   = 0x00;                            //Desliga PORTD
        dez = 2;                                   //Calcula o dígito das dezenas
        digitalWrite(dig_dez, HIGH);               //Ativa dígito das dezenas
        
        if(flagAux) PORTD   = display_f(dez);                  //Escreve o valor no display das dezenas
        else PORTD = display_v(dez);
        
    } //end else if dig_dez
        
    else if(!digitalRead(dig_uni) && control == 4) //Dígito das unidades desligado?
    {                                              //Variável de controle igual a 4?
        control = 0x01;                            //Sim, control recebe o valor 1 (para encerrar a lógica)
        digitalWrite(dig_dez, LOW);                //Apaga o dígito das dezenas
        digitalWrite(dig_cen, LOW);                //Apaga o dígito das centenas
        digitalWrite(dig_mil, LOW);                //Apaga o dígito dos milhares
        PORTD   = 0x00;                            //Desliga PORTD
        uni = 3;                                   //Calcula o dígito das unidades
        digitalWrite(dig_uni, HIGH);               //Ativa dígito das unidades
        
        if(flagAux) PORTD   = display_f(uni);                  //Escreve o valor no display das unidades
        else PORTD = display_v(uni);
        
     } //end else if dig_uni
     
} //end Timer2 OVF




//========================================================================================//
// --- Configurações Iniciais ---
void setup()
{
     for(short i=0x00;i<0x08;i++) pinMode(i, OUTPUT); //saída para os dados do display
     
     pinMode(dig_uni, OUTPUT); //saída de controle do display mais significativo
     pinMode(dig_dez, OUTPUT); //.
     pinMode(dig_cen, OUTPUT); //.
     pinMode(dig_mil, OUTPUT); //saída de controle do display menos significativo
     
     pinMode(sensor, INPUT_PULLUP); //entrada para o sensor de nível
     pinMode(bomba, OUTPUT); //saída para controle da bomba d'agua
     
     
     digitalWrite(bomba, LOW); //bomba inicia desligada
     
     
     // -- Configuração do Estouro do Timer 2 --
     TCCR2A = 0x00;   //Timer operando em modo normal
     TCCR2B = 0x07;   //Prescaler 1:1024
     TCNT2  = 196;    //Inicia conteúdo do Timer2 em 196d
     TIMSK2 = 0x01;   //Habilita interrupção do Timer2


} //end setup


//========================================================================================//
// --- Loop Infinito ---
void loop()
{
   if(!digitalRead(sensor)) 
   {
      digitalWrite(bomba, LOW); //desliga bomba
      flagAux = 0x01;  //se sensor acionado, seta flagAux
   } //end if
   
   else 
   {
     digitalWrite(bomba, HIGH); //liga bomba 
     flagAux = 0x00; //senão, limpa flag
   } //end else
 

} //end loop


//========================================================================================//
// --- Desenvolvimento das Funções ---

//========================================================================================//
// --- Exibe FULL no display --
int display_f(int num)
{
    int cathode;                               //armazena código BCD

    //-- Vetor para o código BCD --
    int SEGMENTO[4] = {0x71,                  //"F"
                       0x3E,                  //"U"
                       0x38,                  //"L"
                       0x38                   //"L"
                       };                

    cathode = SEGMENTO[num];                   //para retornar o cathode

    return(cathode);                           //retorna o número BCD

} //end display


//========================================================================================//
// --- Exibe UOId (void) no display ---
int display_v(int num)                      
{
    int cathode;                               //armazena código BCD

    //Vetor para o código BCD
    int SEGMENTO[4] = {0x3E,                  //"U"
                       0x3F,                  //"O"
                       0x06,                  //"I"
                       0x5E                   //"d"
                       };                

    cathode = SEGMENTO[num];                   //para retornar o cathode

    return(cathode);                           //retorna o número BCD

} //end display







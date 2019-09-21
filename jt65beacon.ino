#define carrier 14.076e6
#define synctone 1270.5
#define offset_multiplier 1 //value of m

unsigned long deltaphase = 0;
float cal_factor = 1.000015;

/*
Each channel symbol generates a tone at frequency 1270.5 + 2.6917 (N+2) m Hz, where N is
 the integral symbol value, 0 ≤ N ≤63, and m assumes the values 1, 2, and 4 for JT65 sub-modes A, B, and C.
 */

int sync_vector[] = {
    1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0,
    0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1,
    0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1};

int cq_call[] = {
    57, 17, 9, 46, 0, 20, 63, 5, 20, 2, 41, 1, 0, 60, 31, 63, 32, 6, 37, 30, 0,
    42, 44, 46, 58, 33, 26, 2, 22, 37, 34, 4, 20, 27, 48, 6, 48, 2, 28, 5, 7, 20,
    9, 48, 54, 57, 19, 48, 24, 2, 50, 51, 41, 4, 51, 34, 4, 52, 27, 58, 16, 53, 46};

void setup()
{
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);
}

void loop()
{

    dds_send(0);
    delay(200);
    int index = 0;
    for (int i = 0; i < 126; i++)
    {
        int x = sync_vector[i];

        if (x == 1)
        {
            //generate the sync tone
            double frequency = carrier + synctone;
            //send it to the dds
            dds_send(frequency);
        }

        else
        {
            // it's not a sync tone so calculate the correct tone for this point of the call
            int symbol = cq_call[index];
            double offset = synctone + (2.6917 * (symbol + 2) * offset_multiplier); // frequency 1270.5 + 2.6917 (N+2) m Hz
            double frequency = carrier + offset;

            index++;
            //send it to the dds
            dds_send(frequency);
        }

        delay(367);
    }
    double frequency = 0;
    dds_send(frequency);
    delay(71.7e3);
}

// Functions...

void dds_send(double frequency)
{
    deltaphase = ((cal_factor * frequency * 4294967296LL) / 125000000LL);

    updatefreq(deltaphase);
}

void updatefreq(long deltaphase)
{
    for (int i = 0; i < 40; i++)
    {
        digitalWrite(8, (bitRead(deltaphase, i)));
        //  delay(50);
        digitalWrite(7, HIGH);
        //  delay(50);
        digitalWrite(7, LOW);
    }
    digitalWrite(6, HIGH);
    // delay(100);
    digitalWrite(6, LOW);
}

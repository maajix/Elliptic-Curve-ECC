#include <iostream>
#include <vector>

using namespace std;


struct Point
{
    int x;
    int y;

    bool operator == (Point Q)
    {
        if (x == Q.x && y == Q.y)
            return true;
        else
            return false;
    }
};

class ELCurve
{
private:
    int a, b;
    int mod;
    int x1, x2, y1, y2;
    int s;
public:
    ELCurve(int A = 0, int B = 0, int MOD = 0) : a(A), b(B), mod(MOD) 
    {
        x1 = 0;
        x2 = 0;
        y1 = 0;
        y2 = 0;
        s = 0;
        if (!isSecure())
            cout << "Curve is not secure!" << endl;
    }

    int modInverse(int a, int m)
    {
        a = a % m;
        for (int x = -m; x < m; x++)
        {
            if (a < 0)
                while (a < 0)
                    a += m;

            if ((a * x) % m == 1)
                return x;
        }
        return 0;
    }

    Point pCalc (Point P, Point Q)
    {
        x1 = P.x % mod;
        y1 = P.y % mod;
        x2 = Q.x % mod;
        y2 = Q.y % mod;

        //O := NULL 
        if (x1 == NULL && y1 == NULL)
            return { x2,y2 };
        if (x2 == NULL && y2 == NULL)
            return { x1,y1 };

        //=> P + (-P) = O := {-1, -1}
        if (x1 == x2 && y1 == -y2)
            return { -1, -1 };

        //==PointAddition==
        if (x1 != x2 || y1 != y2)
        {
            // =Dividing by 0=
            if (x1 == x2) 
                return { -1, -1 };

            // s = (y2 - y1) : (x2 - x1) % mod p
            s = ((y2 - y1) * modInverse((x2 - x1), mod)) % mod;
            //==Make sure s is positive==
            if (s < 0)
                while (s < 0)
                    s += mod;
        }
        //==PointDouble==
        else if (x1 == x2 && y1 == y2)
        {
            // s = (3*x1^2) : (2*y1) mod p
            s = ((3 * (x1 * x1) + a) * modInverse((2 * y1), mod)) % mod;
            //==Make sure s is positive==
            if (s < 0)
                while (s < 0)
                    s += mod;
        }
        else
            cout << "[ERROR] COULD NOT COMPUTE:  (X1/Y1): ("<< x1 << "/"<<y1<<") (X2/Y2): ("<<x2<<"/"<<y2 << ") s: "<<s << endl;

        //==Get new XY Position==
        int x3, y3;
        x3 = ((s * s) - x1 - x2) % mod;
        y3 = (s * (x1 - x3) - y1) % mod;

        //==If Negative get Positive==
        if (x3 < 0)
            while (x3 < 0)
                x3 += mod;

        if (y3 < 0)
            while (y3 < 0)
                y3 += mod;

        return { x3, y3 };
    }
    Point pCalc (int mul, Point Q)
    {
        Point tmp = Q;
        for (int i = 1; i < mul; i++)
        {
            //cout << "("<<tmp.x << "/" << tmp.y << ") + (" << Q.x << "/" << Q.y << ")"<< endl;
            tmp = pCalc(tmp, Q);
        }
        return tmp;
    }

    bool isSecure()
    {
        if ((4 * (a * a * a) + 27 * (b * b)) % mod != 0)
            return true;
        else
            return false;
    }
    bool isPoint(Point P)
    {
        if ((P.y * P.y) == (P.x * P.x * P.x + a * P.x + b) % mod)
            return true;
        else
            return false;
    }

    vector<Point> findAllPoints(bool print_Table = false, bool print_finalPoints = false)
    {
        // (x = index , y = value)

        vector<Point>x_lst;
        for (int i = 0; i < mod; i++)
        {
            x_lst.push_back({ i, (((i * i * i) + a * i + b) % mod) });
        }

        vector<Point>y_lst;
        for (int i = 0; i < mod; i++)
        {
            y_lst.push_back({ i, ((i * i) % mod) });
        }

        vector<Point>finPoints;
        for (unsigned int i = 0; i < x_lst.size(); i++)
        {
            for (unsigned int k = 0; k < y_lst.size(); k++)
            {
                if (x_lst[i].y == y_lst[k].y)
                {
                    finPoints.push_back({ x_lst[i].x, y_lst[k].x });
                }
            }
        }

        //==PRINT Y | X TABLE==
        if (print_Table)
        {
            cout << endl;
            cout << "Y | Y^2" << endl;
            for (Point s : y_lst)
                cout << "(" << s.x << " " << s.y << ")" << endl;
            cout << endl;
            cout << "X | X^3 + " << a << "X + " << b << " mod " << mod << endl;
            for (Point a : x_lst)
                cout << "(" << a.x << " " << a.y << ")" << endl;
            cout << endl;
        }

        //==PRINT ALL MATCHING POINTS==
        if (print_finalPoints)
        {
            cout << endl;
            for (Point a : finPoints)
                cout << "=> (" << a.x << "/" << a.y << ")" << endl;
            cout << endl;
        }

        return finPoints;
    }
    int getOrder(vector<Point> P)
    {
        return P.size() + 1;
    }

    void print_data(Point P)
    {
        cout << "(" << P.x << "/" << P.y << ")" << endl;
    }  
};

class DHKey
{
private:
    ELCurve e;
    Point P;
    int a;
    int b;

public:
    DHKey(ELCurve E, Point P, int a_prv, int b_prv) : e(E), P(P), a(a_prv), b(b_prv) 
    {
        if (!(a > 1 && a < e.getOrder(e.findAllPoints()) - 1))
            a = 0;
        if (!(b > 1 && b < e.getOrder(e.findAllPoints()) - 1))
            b = 0;
    }

    Point calc_Pub_A()
    {
        return e.pCalc(a, P);
    }

    Point calc_Pub_B()
    {
        return e.pCalc(b, P);
    }

    Point calc_Pub_AB()
    {
        return e.pCalc(a, calc_Pub_B());
    }

    bool AB_check()
    {
        if (e.pCalc(b, calc_Pub_A()) == calc_Pub_AB())
            return true;
        else
            return false;   
    }

    void fullExchange(bool log = true)
    {
        if (log)
        {
            Point PA = calc_Pub_A();
            cout << "Pub(A) = (" << PA.x << "/" << PA.y << ")" << endl;
            Point PB = calc_Pub_B();
            cout << "Pub(B) = (" << PB.x << "/" << PB.y << ")" << endl;
            Point PAB = calc_Pub_AB();
            cout << "Pub(AB) = (" << PAB.x << "/" << PAB.y << ")" << endl;
            cout << "Checking: ";
            if (AB_check())
                cout << "{Correct}";
            else
                cout << "{FAILED} Check Parameters!";
        }
    }
};

int main()
{
    ELCurve ecc(2, 2, 17);
    
    // Finde alle möglichen Punkte auf der Kurve
    // ecc.findAllPoints(false, true); 

    /*
    alle punkte auf der kurve
    1  (0/6)
    2  (9/1)
    3  (6/3)
    4  (7/6)
    5  (10/11)
    6  (3/1)
    7  (13/10)
    8  (5/16)
    9  (16/13)
    10 (16/4)
    11 (5/1)
    12 (13/7)
    13 (3/16)
    14 (10/6)
    15 (7/11)
    16 (6/14)
    17 (9/16)
    18 (0/11)

    [1] ri = 3 oder 16 

    MÖGLICHE A (schauen in liste):
    6:  (3/1)
    9:  (16/13)
    10: (16/4)
    13: (3/16)

    phi(ri * P) 
    (3*{0,6})  = (6,3)  = 6
    (16*{0,6}) = (6,14) = 6 

    si+1 = 6

    ri = phi(si *P)
    ri = phi(6, {0,6}) = {3,1} = 3

    ti = phi(ri * Q)
    ti = phi(3 * {7,11}) = {13, 10} = 13 mod 8 = 5 CHECKED
    */

    // [1]
    /* ri bestimmung (probieren aller x koordinaten)
    Point x  = ecc.pCalc(0, Q);
    Point x1 = ecc.pCalc(3, Q);
    Point x2 = ecc.pCalc(5, Q);
    Point x3 = ecc.pCalc(6, Q);
    Point x4 = ecc.pCalc(7, Q);
    Point x5 = ecc.pCalc(9, Q);
    Point x6 = ecc.pCalc(10, Q);
    Point x7 = ecc.pCalc(13, Q);
    Point x8 = ecc.pCalc(16, Q);
    ecc.print_data(x);
    ecc.print_data(x1);
    ecc.print_data(x2);
    ecc.print_data(x3);
    ecc.print_data(x4);
    ecc.print_data(x5);
    ecc.print_data(x6);
    ecc.print_data(x7);
    ecc.print_data(x8);
    */

    ecc.findAllPoints(false, true);

    Point si1_1 = ecc.pCalc(3, { 0,6 });
    ecc.print_data(si1_1); 
    cout << "si+1(1) x: " << si1_1.x << endl << endl;

    Point si1_2 = ecc.pCalc(16, { 0,6 });
    ecc.print_data(si1_2);
    cout << "si+1(2) x: " << si1_2.x << endl << endl;

    Point ri = ecc.pCalc(6, { 0,6 });
    ecc.print_data(ri);
    cout << "ri x: " << ri.x << endl << endl;

    Point ti = ecc.pCalc(3, { 7,11 });
    ecc.print_data(ti);
    cout << "ti x: " << ti.x << endl << endl;
    cout << "CHECK: " << ti.x << " mod 8 = "<< ti.x % 8 << endl;


    std::cin.get();
}

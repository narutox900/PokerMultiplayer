using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class Cards : MonoBehaviour
{
    public Sprite[] cardSprites = new Sprite[54]; 

    //card string-form H1
    public int IntToCardIndex(bool back, int rank, int suit)
    {
        if(back == true)
        {
            return 0;
        }
        return (rank - 2) * 4 + suit + 1;
    }
}

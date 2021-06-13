using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class SliderHandler : MonoBehaviour
{
    public Slider slider;
    public Text text;
    public int maxValue;
    public int minValue;


    private void OnEnable()
    {
        slider.maxValue = maxValue;
        slider.minValue = minValue;
        text.text = "$" + minValue;
    }

    private void Start()
    {
    }
    public void OnValueChanged()
    {
        text.text = "$" + slider.value;
    }
}

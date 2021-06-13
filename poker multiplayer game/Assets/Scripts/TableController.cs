using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using Network;
using Game;
using System.Threading;

public class TableController : MonoBehaviour
{

    CommunicationHandler communicationHandler;
    ClientController clientController;

    public GameObject[] playerList = new GameObject[6]; //player list
    public GameObject[] communityCardList = new GameObject[5];
    private int myID = -1;
    private int myRoomID = -1;
    private int playerCount = -1;

    //public Slider betSlider;
    //public Slider raiseSlider;

    public GameObject betOption1;   //check bet fold
    public GameObject betOption2;   //call raise fold
    public GameObject betOption3;   //allin fold 

    public GameObject startButton;

    Cards cards;
    void Awake()
    {
        foreach(var player in playerList)
        {
            player.SetActive(false);
        }
    }
    // Start is called before the first frame update
    void Start()
    {
        //communicationHandler.BroadcastMessage("StartConnectToServer");
        //communicationHandler.BroadcastMessage("PrepareSendMessage", 1);
        //communicationHandler.BroadcastMessage("SetDisconnect");

        //get myID and playercount from server
        //myID = 0;
        //playerCount = 6;
        //TODO: enable player gameobject and render
        //bla bla

        cards = GameObject.Find("Cards").GetComponent<Cards>();
        communicationHandler = GameObject.Find("Communication Handler").GetComponent<CommunicationHandler>();
        clientController = GameObject.Find("Client Controller").GetComponent<ClientController>();


        //RenderCard(false, (int)11, (int)1, communityCardList[0]);
        //RenderCard(false, (int)12, (int)2, communityCardList[1]);
        //for test
        //RenderMoney(200000, 1);
        //DisplayBetOptions(1, 20, 200);
        //RenderPlayerCall(0, 0, 0);

    }

    // Update is called once per frame
    void Update()
    {
    }


    //method to render game components
    private int PlayerFromID(int ID) //ID from server
    {
        return (ID - myID) >= 0 ? (ID - myID) : (playerCount + ID - myID);
        //return playerCount + ID;
    }

    private void RenderCard(bool back, int rank, int suit, GameObject cardGo)     //use when want to render card on table
    {
        cardGo.GetComponent<SpriteRenderer>().sprite = cards.cardSprites[cards.IntToCardIndex(back, rank, suit)];
    }

    private void RenderMoney(int money, int ID)     //use to render player's current money
    {
        playerList[PlayerFromID(ID)].transform.GetChild(4).transform.GetChild(0).GetComponent<Text>().text = "$" + money;
    }

    private void RenderBetMoney(int money, int ID, int option)
    {
        switch(option)
        {
            case 0:
                playerList[PlayerFromID(ID)].transform.GetChild(4).transform.GetChild(2).GetComponent<Text>().text = "C : $" + money;
                break;
            case 1:
                playerList[PlayerFromID(ID)].transform.GetChild(4).transform.GetChild(2).GetComponent<Text>().text = "R: $" + money;
                break;
            case 2:
                playerList[PlayerFromID(ID)].transform.GetChild(4).transform.GetChild(2).GetComponent<Text>().text = "";
                break;
            case 3:
                playerList[PlayerFromID(ID)].transform.GetChild(4).transform.GetChild(2).GetComponent<Text>().text = "Init: $5";
                break;
            case 4:
                playerList[PlayerFromID(ID)].transform.GetChild(4).transform.GetChild(2).GetComponent<Text>().text = "+ $" + money;
                break;
            default:
                break;
                
        }
        
    }
    
    private void DisplayBetOptions(int betOption, int min, int max)     //call when your turn start
    {
        switch (betOption)
        {
            case 1:
                betOption1.transform.GetChild(3).GetComponent<SliderHandler>().minValue = min;
                betOption1.transform.GetChild(3).GetComponent<SliderHandler>().maxValue = max;
                betOption1.SetActive(true);
                break;
            case 2:
                betOption2.transform.GetChild(3).GetComponent<SliderHandler>().minValue = min;
                betOption2.transform.GetChild(3).GetComponent<SliderHandler>().maxValue = max;
                betOption2.SetActive(true);
                break;
            case 3:
                betOption3.SetActive(true);
                break;
            default:
                break;
        }
    }

    public void OnCall() //call when bet/raise button pushed
    {
        communicationHandler.PrepareSendMessage(5, 0, 0);
        communicationHandler.StartCoroutine("SendMessage");
    }

    public void OnRaise() //call when fold button pushed
    {
        communicationHandler.PrepareSendMessage(5, 1, (int)betOption2.transform.GetChild(3).GetComponent<Slider>().value);
        communicationHandler.StartCoroutine("SendMessage");
    }

    public void OnFold() //call when all in button pushed
    {
        communicationHandler.PrepareSendMessage(5, 2, 0);
        communicationHandler.StartCoroutine("SendMessage");
    }
    

    public void OnExit()
    {

    }

    public void SetId(int id)
    {
        this.myID = id;
    }

    public void SetRoomID(int roomID)
    {
        this.myRoomID = roomID;
    }

    public void StartGame()
    {
        communicationHandler.PrepareSendMessage(0, 0, 0);
        communicationHandler.StartCoroutine("SendMessage");
    }

    public void RenderPlayer(RoomInfo info)
    {
        Debug.Log("TC Room Info");
        if(info.Clients.Count > playerCount)
            playerCount = info.Clients.Count;
        foreach (var player in playerList)
        {
            player.SetActive(false);
        }

        foreach (var client in info.Clients)
        {
            Debug.Log("Playerid: " + client.Id + " name: " + client.Name);
            playerList[PlayerFromID((int)client.Id)].SetActive(true);
            playerList[PlayerFromID((int)client.Id)].transform.GetChild(4).transform.GetChild(1).GetComponent<Text>().text = client.Name;
        }
    }
    
    public void StartGame(StartGameResponse startGameResponse)
    {
        if(startGameResponse.Success == true)
        {
            foreach(var player in startGameResponse.Players)
            {
                RenderMoney((int)player.Balance, (int)player.Id);
                RenderBetMoney(5, (int)player.Id, 3);
            }
        }
        else
        {
            Debug.Log("start game failed?!");
        }
    }

    public void DealCard(DealCards dealCards)
    {
        foreach(var player in playerList)
        {
            if(player.activeInHierarchy == true)
            {
                RenderCard(true, 0, 0, player.transform.GetChild(2).gameObject);
                RenderCard(true, 0, 0, player.transform.GetChild(3).gameObject);
            }
        }
        int i = 2;
        foreach(var card in dealCards.Cards)
        {
            RenderCard(false, (int)card.Value, (int)card.Suit, playerList[PlayerFromID(myID)].transform.GetChild(i).gameObject);
            i++;
        }
    }

    public void HandleBetTurn(BetTurn betTurn)
    {
        playerList[PlayerFromID((int)betTurn.PlayerId)].transform.GetChild(5).gameObject.SetActive(true);
        if((int)betTurn.PlayerId == myID)
        {
            DisplayBetOptions(2, (int)betTurn.Amount, (int)betTurn.Balance);

        }
    }

    public void DealCommunityCard(DealCommunityCards dealCommunityCards)
    {
        switch((int)dealCommunityCards.Phase)
        {
            case 1:
                RenderCard(false, (int)dealCommunityCards.Cards[0].Value, (int)dealCommunityCards.Cards[0].Suit, communityCardList[0]);
                RenderCard(false, (int)dealCommunityCards.Cards[1].Value, (int)dealCommunityCards.Cards[1].Suit, communityCardList[1]);
                break;
            case 2:
            case 3:
            case 4:
                RenderCard(false, (int)dealCommunityCards.Cards[0].Value, (int)dealCommunityCards.Cards[0].Suit, communityCardList[(int)dealCommunityCards.Phase]);
                break;
            default:
                RenderCard(true, 12, 2, communityCardList[0]);
                break;

        }
        Thread.Sleep(3000);
    }

    public void HandleDoneBet(DoneBet doneBet)
    {

        playerList[PlayerFromID((int)doneBet.PlayerId)].transform.GetChild(5).gameObject.SetActive(false);
        betOption2.SetActive(false);
        RenderMoney((int)doneBet.PlayerBalance, (int)doneBet.PlayerId);
        switch(doneBet.Action)
        {
            case 0:
                RenderBetMoney((int)doneBet.BetAmount, (int)doneBet.PlayerId, 0);
                break;
            case 1:
                RenderBetMoney((int)doneBet.BetAmount, (int)doneBet.PlayerId, 1);
                break;
            case 2:
                RenderBetMoney((int)doneBet.BetAmount, (int)doneBet.PlayerId, 2);
                playerList[PlayerFromID((int)doneBet.PlayerId)].transform.GetChild(6).gameObject.SetActive(true);
                break;
        }

    }

    public void HandleEndRound(EndRound endRound)
    {
        for(int i = 0; i < playerCount; i++)
        {
            RenderBetMoney(0, PlayerFromID(i), 2);
        }
        //Thread.Sleep(3000);
    }

    public void HandleResult(Result result)
    {
        playerList[PlayerFromID((int)result.WinnerId)].transform.GetChild(7).gameObject.SetActive(true);
        RenderBetMoney((int)result.Prize, (int)result.WinnerId, 4);
        foreach(var player in result.EndPlayers)
        {
            int i = 2;
            foreach (var card in player.Cards)
            {
                RenderCard(false, (int)card.Value, (int)card.Suit, playerList[PlayerFromID((int)player.Player.Id)].transform.GetChild(i).gameObject);
                i++;
            }
        }


        StartCoroutine(ResetGame());

    }

    IEnumerator ResetGame()
    {
        yield return new WaitForSeconds(6);
        foreach(var player in playerList)
        {
            
                RenderCard(false, 14, 4, player.transform.GetChild(2).gameObject);
                RenderCard(false, 14, 4, player.transform.GetChild(3).gameObject);
                player.transform.GetChild(5).gameObject.SetActive(false);
                player.transform.GetChild(6).gameObject.SetActive(false);
                player.transform.GetChild(7).gameObject.SetActive(false);
            
        }
        foreach(var card in communityCardList)
        {
            RenderCard(false, 14, 4, card);
        }

    }

    public void Exit()
    {
        clientController.PrepareSendMessage(new Vector2Int(6, myRoomID));
        clientController.StartCoroutine("SendMessage");
    }
}

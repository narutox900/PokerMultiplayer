using System.Collections;
using UnityEngine;
using System.Runtime.InteropServices;
using System;
using Network;
using Game;
using Google.Protobuf;

public class CommunicationHandler : MonoBehaviour
{
    [DllImport("D:\\Game\\unity\\poker multiplayer game\\x64\\Debug\\client_sock.dll")]
    public static extern int Connect(string ipaddr, string port);
    [DllImport("D:\\Game\\unity\\poker multiplayer game\\x64\\Debug\\client_sock.dll")]
    public static extern int Send(int sockfd, byte[] sendmess, int size);
    [DllImport("D:\\Game\\unity\\poker multiplayer game\\x64\\Debug\\client_sock.dll")]
    public static extern int Receive(int sockfd, byte[] receivmess, int size);
    [DllImport("D:\\Game\\unity\\poker multiplayer game\\x64\\Debug\\client_sock.dll")]
    public static extern int Close(int sockfd);

    public TableController tableController;
    

    private int id;
    private string clientName;
    int sockfd = -1;
    static byte[] recvbuff;
    static byte[] sendbuff;
    static int sendSize = 0;
    static int recvSize = 4;
    
    

    void Awake()
    {
        tableController = GameObject.Find("Table Controller").GetComponent<TableController>();
    }
    void Start()
    {
        StartCoroutine(WaitForRespond());
    }

    public int MyId()
    {
        return id;
    }

    public void SetMyId(int id)
    {
        this.id = id;
    }

    public string MyName()
    {
        return clientName;
    }

    public void SetMyName(string name)
    {
        this.clientName = name;
    }


    /*------------------------------------------------------*/
    public IEnumerator ConnectToServer(string port)
    {
        while ((sockfd = Connect("127.0.0.1", port)) == -1)
        {
            Debug.Log("CH Connecting...");
            yield return new WaitForSeconds(.5f);
        }
        Debug.Log("CH Connected");
    }

    /*------------------------------------------------------*/

    public IEnumerator WaitForRespond()
    {
        for (; ; )
        {
            yield return new WaitForSeconds(.5f);
            if (sockfd == -1)
            {
                Debug.Log("Not listening!");
                continue;
            }
                
            int n = 0;
            recvSize = 4;
            recvbuff = new byte[recvSize];
            if ((n = Receive(sockfd, recvbuff, recvSize)) > 0)
            {

                Debug.Log("CH received " + n + " bytes");
                int payLoadsize = BitConverter.ToInt32(recvbuff, 0);
                Debug.Log("CH payload size: " + payLoadsize + " bytes");
                HandleMessage(payLoadsize);
            }
            else
            {
                //Debug.Log("Nothing here. Gotta wait!");
            }
            
        }
    }


    /*------------------------------------------------------*/
    public IEnumerator SendMessage()
    {
        while (sockfd == -1)
        {
            yield return null;
        }

        int n;
        if ((n = Send(sockfd, sendbuff, sendSize)) > 0)
        {
            Debug.Log("CH sent message!");
            Array.Clear(sendbuff, 0, sendbuff.Length);
        }
        else
        {
            Debug.Log("CH sent message failed!");
        }

    }

    /*------------------------------------------------------*/
    public void SetDisconnect()
    {
        Close(sockfd);
    }

    /*------------------------------------------------------*/
    public void HandleMessage(int size)
    {
        //get message type
        recvSize = 1;
        recvbuff = new byte[1];
        int type = Receive(sockfd, recvbuff, recvSize);

        type = Convert.ToInt32(recvbuff[0]);
        Debug.Log("CH message type: " + type);

        switch (type)
        {
            //start game response
            case 1:
                //read payload
                if (size != 0)
                {
                    recvSize = size;
                    recvbuff = new byte[recvSize];
                    int n = Receive(sockfd, recvbuff, recvSize);
                    Debug.Log("CH received " + n + " payload bytes");
                    Debug.Log("CH handle response 1");

                    StartGameResponse startGameResponse = new StartGameResponse { };
                    startGameResponse = StartGameResponse.Parser.ParseFrom(recvbuff);
                    tableController.StartGame(startGameResponse);

                    
                }
            break;
            //deal card
            case 2:
                
                if (size != 0)
                {
                    recvSize = size;
                    recvbuff = new byte[recvSize];
                    int n = Receive(sockfd, recvbuff, recvSize);
                    Debug.Log("CH received " + n + " payload bytes");

                    Debug.Log("CH handle response 2");

                    DealCards dealCards = new DealCards { };
                    dealCards = DealCards.Parser.ParseFrom(recvbuff);
                    tableController.DealCard(dealCards);

                }
                break;
            //deal community card
            case 3:
                if (size != 0)
                {
                    recvSize = size;
                    recvbuff = new byte[recvSize];
                    int n = Receive(sockfd, recvbuff, recvSize);
                    Debug.Log("CH received " + n + " payload bytes");

                    Debug.Log("CH handle response 3");

                    DealCommunityCards dealCommunityCards = new DealCommunityCards { };
                    dealCommunityCards = DealCommunityCards.Parser.ParseFrom(recvbuff);
                    tableController.DealCommunityCard(dealCommunityCards);
                }
                break;
            //bet turn
            case 4:
                if (size != 0)
                {
                    recvSize = size;
                    recvbuff = new byte[recvSize];
                    int n = Receive(sockfd, recvbuff, recvSize);
                    Debug.Log("CH received " + n + " payload bytes");

                    Debug.Log("CH handle response 4");
                    BetTurn betTurn = new BetTurn { };
                    betTurn = BetTurn.Parser.ParseFrom(recvbuff);
                    tableController.HandleBetTurn(betTurn);

                }
                break;
            //done bet
            case 6:
                
                if (size != 0)
                {
                    recvSize = size;
                    recvbuff = new byte[recvSize];
                    int n = Receive(sockfd, recvbuff, recvSize);
                    Debug.Log("CH received " + n + " payload bytes");

                    Debug.Log("CH handle response 6");
                    DoneBet doneBet = new DoneBet { };
                    doneBet = DoneBet.Parser.ParseFrom(recvbuff);
                    tableController.HandleDoneBet(doneBet);
                }
                break;
            //end round
            case 7:
                
                if (size != 0)
                {
                    recvSize = size;
                    recvbuff = new byte[recvSize];
                    int n = Receive(sockfd, recvbuff, recvSize);
                    Debug.Log("CH received " + n + " payload bytes");

                    Debug.Log("CH handle response 7");
                    EndRound endRound = new EndRound { };
                    endRound = EndRound.Parser.ParseFrom(recvbuff);

                    tableController.HandleEndRound(endRound);

                }
                break;
            //result
            case 8:
                if (size != 0)
                {
                    recvSize = size;
                    recvbuff = new byte[recvSize];
                    int n = Receive(sockfd, recvbuff, recvSize);
                    Debug.Log("CH received " + n + " payload bytes");

                    Debug.Log("CH handle response 8");
                    Result result = new Result { };
                    result = Result.Parser.ParseFrom(recvbuff);
                    tableController.HandleResult(result);

                }
                break;

            default:
                break;
        }

    }

    /*------------------------------------------------------*/
    public void PrepareSendMessage(int i, int optional1, int optional2)    //since unity only allowed passing 1 param? :D
                                             //x = flag, y = value(optional)
    {
        switch (i)
        {
            //start game request
            case 0:
                Game.StartGameRequest startGame = new Game.StartGameRequest { };
                startGame.Id = (uint)id;
                startGame.Success = true;
                byte[] startGameMess = startGame.ToByteArray();
                sendbuff = new byte[startGameMess.Length + 5];
                sendSize = startGameMess.Length + 5;


                Array.Copy(BitConverter.GetBytes(startGameMess.Length), 0, sendbuff, 0, 4);
                Array.Copy(startGameMess, 0, sendbuff, 5, startGameMess.Length);
                sendbuff[4] = 0;
                

                //TODO?
                break;
            //bet turn response
            case 5:
                BetTurnResponse betTurnResponse = new BetTurnResponse { };
                betTurnResponse.PlayerId = (uint)id;
                betTurnResponse.Action = (uint)optional1;
                betTurnResponse.Amount = (uint)optional2;

                byte[] betTurnResponseMess = betTurnResponse.ToByteArray();
                sendbuff = new byte[betTurnResponseMess.Length + 5];
                sendSize = betTurnResponseMess.Length + 5;

                Array.Copy(BitConverter.GetBytes(betTurnResponseMess.Length), 0, sendbuff, 0, 4);
                Array.Copy(betTurnResponseMess, 0, sendbuff, 5, betTurnResponseMess.Length);
                sendbuff[4] = 5;

                break;
            
            default:
                break;
        }

    }
    
}

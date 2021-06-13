using System.Collections;
using UnityEngine;
using System.Runtime.InteropServices;
using System.Threading;
using Network;
using Google.Protobuf;
using System;
using UnityEngine.SceneManagement;

public class ClientController : MonoBehaviour
{
    [DllImport("D:\\Game\\unity\\poker multiplayer game\\x64\\Debug\\client_sock.dll")]
    public static extern int Connect(string ipaddr, string port);
    [DllImport("D:\\Game\\unity\\poker multiplayer game\\x64\\Debug\\client_sock.dll")]
    public static extern int Send(int sockfd, byte[] sendmess, int size);
    [DllImport("D:\\Game\\unity\\poker multiplayer game\\x64\\Debug\\client_sock.dll")]
    public static extern int Receive(int sockfd, byte[] receivmess, int size);
    [DllImport("D:\\Game\\unity\\poker multiplayer game\\x64\\Debug\\client_sock.dll")]
    public static extern int Close(int sockfd);

    public TableController tableController = null;
    public CommunicationHandler communicationHandler = null;
    public RoomController roomController;
    

    private int id;
    private string clientName = "";
    public int sockfd = -1;
    static byte[] recvbuff;
    static byte[] sendbuff;
    static int sendSize = 0;
    static int recvSize = 4;

    public static ClientController Instance { get; private set; }

    void Awake()
    {
        // Save a reference to the AudioManager component as our //singleton instance.
        if (Instance == null)
        {
            Instance = this;
            DontDestroyOnLoad(gameObject);
        }
        else
        {
            Destroy(gameObject);
        }
    }
    // Instance method, this method can be accessed through the //singleton instance
    void Start()
    {
        

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

    public IEnumerator ConnectToServer(string ip)
    {
        while ((sockfd = Connect(ip, "5000")) == -1)
        {
            Debug.Log("CC Connecting...");
            yield return new WaitForSeconds(.5f);
        }
        Debug.Log("CC Connected");
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

                Debug.Log("CC received " + n + " bytes");
                int payLoadsize = BitConverter.ToInt32(recvbuff, 0);
                Debug.Log("CC payload size: " + payLoadsize + " bytes");
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

        if (Send(sockfd, sendbuff, sendSize) > 0)
        {
            Debug.Log("CC sent message!");
            Array.Clear(sendbuff, 0, sendbuff.Length);
        }
        else
        {
            Debug.Log("CC sent message failed!");
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
        Debug.Log("CC message type: " + type);

        switch (type)
        {
            // show room response
            case 1:
                //read payload
                if (size != 0)
                {
                    recvSize = size;
                    recvbuff = new byte[recvSize];
                    int n = Receive(sockfd, recvbuff, recvSize);
                    Debug.Log("CC received " + n + " payload bytes");

                    QueryRoomInfoResponse queryRoomInfoResponse = new QueryRoomInfoResponse { };
                    queryRoomInfoResponse = QueryRoomInfoResponse.Parser.ParseFrom(recvbuff);
                    roomController.ShowRoom(queryRoomInfoResponse);
                    break;
                }
                else
                {
                    roomController.ShowRoom(null);
                    Debug.Log("CC handle case 1");
                    break;
                }
            //create room response
            case 3:
                //read payload
                if (size != 0)
                {
                    recvSize = size;
                    recvbuff = new byte[recvSize];
                    int n = Receive(sockfd, recvbuff, recvSize);
                    Debug.Log("CC received " + n + " payload bytes");

                    Debug.Log("CC handle case 3");
                    CreateRoomResponse createRoomResponse = new CreateRoomResponse { };
                    createRoomResponse = CreateRoomResponse.Parser.ParseFrom(recvbuff);
                    int roomId;
                    if (createRoomResponse.Success == true)
                    {
                        id = (int)createRoomResponse.AssignedIdentity.ClientId;
                        roomId = (int)createRoomResponse.AssignedIdentity.RoomId;
                        int newPort = 5000 + roomId + 1;

                        Debug.Log("Client: " + id + " room: " + roomId);
                        StartCoroutine(LoadAsyncScene("Table"));

                        StartCoroutine(InitiateNewScene(newPort.ToString()));
                    }
                }
                break;
            //join room response
            case 5:
                //read payload
                if (size != 0)
                {
                    recvSize = size;
                    recvbuff = new byte[recvSize];
                    int n = Receive(sockfd, recvbuff, recvSize);
                    Debug.Log("CC received " + n + " payload bytes");

                    Debug.Log("CC handle case 5");
                    JoinRoomResponse joinRoomResponse = new JoinRoomResponse { };
                    joinRoomResponse = JoinRoomResponse.Parser.ParseFrom(recvbuff);
                    int roomId;
                    if (joinRoomResponse.Success == true)
                    {
                        id = (int)joinRoomResponse.AssignedIdentity.ClientId;
                        roomId = (int)joinRoomResponse.AssignedIdentity.RoomId;
                        int newPort = 5000 + roomId + 1;

                        Debug.Log("Client: " + id + " room: " + roomId);
                        StartCoroutine(LoadAsyncScene("Table"));

                        StartCoroutine(InitiateNewScene(newPort.ToString()));
                    }
                }
                break;
            //Leave Room Response
            case 7:
                if (size != 0)
                {
                    recvSize = size;
                    recvbuff = new byte[recvSize];
                    int n = Receive(sockfd, recvbuff, recvSize);
                    Debug.Log("CC received " + n + " payload bytes");

                    Debug.Log("CC handle case 7");
                    LeaveRoomResponse leaveRoomResponse = new LeaveRoomResponse { };
                    leaveRoomResponse = LeaveRoomResponse.Parser.ParseFrom(recvbuff);
                    
                    if (leaveRoomResponse.Success == true)
                    {
                        communicationHandler.StopAllCoroutines();
                        communicationHandler.SetDisconnect();
                        StartCoroutine(LoadAsyncScene("Main"));

                    }
                }
                break;
            //roominfo change
            case 8:
                //read payload
                if (size != 0)
                {
                    recvSize = size;
                    recvbuff = new byte[recvSize];
                    int n = Receive(sockfd, recvbuff, recvSize);
                    Debug.Log("CC received " + n + " payload bytes");

                    Debug.Log("CC handle response 8");
                    RoomInfoChanged roomInfoChanged = new RoomInfoChanged { };
                    roomInfoChanged = RoomInfoChanged.Parser.ParseFrom(recvbuff);
                    StartCoroutine(UpdateRoomInfo(roomInfoChanged.NewRoomInfo));




                }
                break;

            default:
                break;
        }

    }


    /*------------------------------------------------------*/
    public void PrepareSendMessage(Vector2Int i)    //since unity only allowed passing 1 param? :D
                                             //x = flag, y = value(optional)
    {
        
        switch (i.x)
        {
            
            case 0:
                QueryRoomInfoRequest queryRoom = new QueryRoomInfoRequest { };
                queryRoom.NotFull = false;

                byte[] queryRoomMess = queryRoom.ToByteArray();
                sendbuff = new byte[queryRoomMess.Length + 5];

                sendSize = queryRoomMess.Length + 5;
                Array.Copy(BitConverter.GetBytes(queryRoomMess.Length), 0, sendbuff, 0, 4);
                sendbuff[4] = 0;
                Array.Copy(queryRoomMess, 0, sendbuff, 5, queryRoomMess.Length);
                
                
                

                break;
            case 2:
                CreateRoomRequest createRoom = new CreateRoomRequest { };
                ClientInfo clientInfoCreateRoom = new ClientInfo { };
                clientInfoCreateRoom.Name = clientName;
                clientInfoCreateRoom.Id = 0;
                
                createRoom.ClientInfo = clientInfoCreateRoom;

                byte[] createRoomMess = createRoom.ToByteArray();
                sendbuff = new byte[createRoomMess.Length + 5];

                sendSize = createRoomMess.Length + 5;
                Array.Copy(BitConverter.GetBytes(createRoomMess.Length), 0, sendbuff, 0, 4);
                sendbuff[4] = 2;
                Array.Copy(createRoomMess, 0, sendbuff, 5, createRoomMess.Length);
                

                break;
            case 4:
                JoinRoomRequest joinRoom = new JoinRoomRequest { };
                ClientInfo clientInfoJoinRoom = new ClientInfo { };
                clientInfoJoinRoom.Id = 0;
                clientInfoJoinRoom.Name = clientName;
                joinRoom.ClientInfo = clientInfoJoinRoom;
                joinRoom.RoomId = (uint)i.y;

                byte[] joinRoomMess = joinRoom.ToByteArray();
                sendbuff = new byte[joinRoomMess.Length + 5];

                sendSize = joinRoomMess.Length + 5;
                Array.Copy(BitConverter.GetBytes(joinRoomMess.Length), 0, sendbuff, 0, 4);
                Array.Copy(joinRoomMess, 0, sendbuff, 5, joinRoomMess.Length);
                sendbuff[4] = 4;
                
                
                break;
            case 6:
                LeaveRoomRequest leaveRoom = new LeaveRoomRequest { };
                ClientIdentity clientIdentity = new ClientIdentity { };
                clientIdentity.RoomId = (uint)i.y;
                clientIdentity.ClientId = (uint)id;
                leaveRoom.ClientIdentity = clientIdentity;

                byte[] leaveRoomMess = leaveRoom.ToByteArray();
                sendbuff = new byte[leaveRoomMess.Length + 5];

                sendSize = leaveRoomMess.Length + 5;
                Array.Copy(BitConverter.GetBytes(leaveRoomMess.Length), 0, sendbuff, 0, 4);
                Array.Copy(leaveRoomMess, 0, sendbuff, 5, leaveRoomMess.Length);
                sendbuff[4] = 6;


                break;
            default:
                break;
        }

    }

    IEnumerator LoadAsyncScene(string scene)
    {
        // The Application loads the Scene in the background as the current Scene runs.
        // This is particularly good for creating loading screens.
        // You could also load the Scene by using sceneBuildIndex. In this case Scene2 has
        // a sceneBuildIndex of 1 as shown in Build Settings.

        AsyncOperation asyncLoad = SceneManager.LoadSceneAsync(scene);

        // Wait until the asynchronous scene fully loads
        while (!asyncLoad.isDone)
        {
            yield return null;
        }
        if(string.Compare(scene, "Main") == 0)
        {
            roomController = GameObject.Find("Room Controller").GetComponent<RoomController>();
        }

    }

    IEnumerator InitiateNewScene(string port)
    {
        if (!SceneManager.GetSceneByName("Table").isLoaded)
        {
            yield return null;
        }

        Debug.Log("Loaded table");
        GameObject go = GameObject.Find("Communication Handler");
        while (go == null)
        {
            go = GameObject.Find("Communication Handler");
            yield return null;
        }
        communicationHandler = go.GetComponent<CommunicationHandler>();
        communicationHandler.StartCoroutine("ConnectToServer", port);

        go = GameObject.Find("Table Controller");
        while (go == null)
        {
            go = GameObject.Find("Table Controller");
            yield return null;
        }
        tableController = go.GetComponent<TableController>();

        
    }

    IEnumerator UpdateRoomInfo(RoomInfo roomInfo)
    {
        //send both to Communication handler and table controller
        while(tableController == null)
        {
            yield return null;
        }

        communicationHandler.SetMyId(id);
        communicationHandler.SetMyName(clientName);
        tableController.SetId(id);
        tableController.SetRoomID((int)roomInfo.Id);
        tableController.RenderPlayer(roomInfo);
        if(id == roomInfo.OwnerId)
        {
            tableController.startButton.SetActive(true);
        }
    }

    public void InitiateClient(string ip)
    {
        //clientName = "Long";
        StartCoroutine(ConnectToServer(ip));
        //PrepareSendMessage(2);
        //StartCoroutine(SendMessage());
    }

}
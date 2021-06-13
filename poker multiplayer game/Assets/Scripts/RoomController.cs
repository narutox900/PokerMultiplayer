using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using Network;

public class RoomController : MonoBehaviour
{

    public GameObject roomUI;
    public GameObject[] roomList = new GameObject[6];
    public ClientController clientController;
    // Start is called before the first frame update
    void Start()
    {
        roomUI = GameObject.Find("Room UI");
        clientController = GameObject.Find("Client Controller").GetComponent<ClientController>();
        if(string.Compare(clientController.MyName(), "") != 0)
        {
            roomUI.transform.GetChild(0).gameObject.SetActive(false);
            roomUI.transform.GetChild(1).gameObject.SetActive(false);
            roomUI.transform.GetChild(2).gameObject.SetActive(false);
            roomUI.transform.GetChild(4).gameObject.SetActive(false);
            roomUI.transform.GetChild(3).gameObject.SetActive(true);
            RequireRoom();
        }
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void RequireRoom()
    {
        clientController.PrepareSendMessage(new Vector2Int(0, 0));
        clientController.StartCoroutine("SendMessage");
    }

    public void ShowRoom(QueryRoomInfoResponse queryRoomInfoResponse)
    {
        foreach (var room in roomList)
        {
            room.transform.GetChild(0).gameObject.SetActive(false);
            room.transform.GetChild(1).gameObject.SetActive(false);
            room.transform.GetChild(2).gameObject.SetActive(true);
            room.GetComponent<Button>().onClick.RemoveAllListeners();
        }

        if (queryRoomInfoResponse == null)
        {
            return;
        }
        
        int i = 0;
        int nob;
        string host = "";
        foreach(var roomInfo in queryRoomInfoResponse.Rooms)
        {
            foreach(var client in roomInfo.Clients)
            {
                if (client.Id == roomInfo.OwnerId)
                    host = client.Name;
            }
            nob = roomInfo.Clients.Count;

            roomList[i].transform.GetChild(0).GetComponent<Text>().text = "Host: " + host;
            if(roomInfo.IsPlaying)
                roomList[i].transform.GetChild(1).GetComponent<Text>().text = "Number of players: " + nob.ToString() + "  (Playing)";
            else
                roomList[i].transform.GetChild(1).GetComponent<Text>().text = "Number of players: " + nob.ToString();
            Debug.Log("Number of players: " + nob.ToString());
            roomList[i].transform.GetChild(0).gameObject.SetActive(true);
            roomList[i].transform.GetChild(1).gameObject.SetActive(true);
            roomList[i].transform.GetChild(2).gameObject.SetActive(false);
            roomList[i].GetComponent<Button>().onClick.AddListener(delegate () { JoinRoom((int)roomInfo.Id); });
            i++;
        }

        
    }

    public void EnterServer()
    {
        StopAllCoroutines();
        clientController.StopAllCoroutines();
        clientController.InitiateClient(roomUI.transform.GetChild(4).transform.GetChild(2).GetComponent<Text>().text);
        StartCoroutine(StartEnterServer());
    }

    IEnumerator StartEnterServer()
    {
        while (clientController.sockfd == -1)
            yield return new WaitForSeconds(1f);
        clientController.StartCoroutine("WaitForRespond");
        string name = roomUI.transform.GetChild(0).transform.GetChild(2).GetComponent<Text>().text;
        roomUI.transform.GetChild(0).gameObject.SetActive(false);
        roomUI.transform.GetChild(1).gameObject.SetActive(false);
        roomUI.transform.GetChild(2).gameObject.SetActive(false);
        roomUI.transform.GetChild(4).gameObject.SetActive(false);
        roomUI.transform.GetChild(3).gameObject.SetActive(true);
        clientController.SetMyName(name);
        RequireRoom();
    }
    

    public void CreateRoom()
    {
        clientController.PrepareSendMessage(new Vector2Int(2, 0));
        clientController.StartCoroutine("SendMessage");
    }

    public void JoinRoom(int roomId)
    {
        clientController.PrepareSendMessage(new Vector2Int(4, roomId));
        clientController.StartCoroutine("SendMessage");
    }
}

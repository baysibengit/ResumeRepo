package com.example.androidchatclient;
//
import android.os.Bundle;
import android.util.Log;
import com.neovisionaries.ws.client.WebSocket;
import com.neovisionaries.ws.client.WebSocketAdapter;
import com.neovisionaries.ws.client.WebSocketException;
import com.neovisionaries.ws.client.WebSocketFactory;
import org.json.JSONObject;
import java.io.IOException;
import java.util.List;
import java.util.Map;

public class MyWebSocket extends WebSocketAdapter {

    final String webSocketLog_ = "MyWebSocket:Bb";
    public static boolean wsIsOpen = false;




    @Override
    public void onConnected(WebSocket websocket, Map<String, List<String>> headers) throws Exception
    {
        Log.d( webSocketLog_, "web socket handshake is complete... we are open");
        wsIsOpen = true;
        ChatRoom.sendJoin();
    }

    @Override
    public void onConnectError(WebSocket websocket, WebSocketException exception) throws Exception
    {
        Log.d("Bb", "error connecting to ws");
    }

    @Override
    public void onTextMessage(WebSocket webSocket, String text) throws Exception {

        super.onTextMessage(webSocket, text);
        Log.d("Bb", text.toString());
       // String userName = ChatRoom.userName_;
        String userName = text.split("\"user\":\"")[1];
        userName = userName.split("\"")[0];
        JSONObject jsonObject = new JSONObject(text);

        String msg = null;

        Log.i("Json Status",jsonObject.toString());
        if( jsonObject.getString("type").equals("join") ){
            msg = userName + " has joined the room";
            Log.d(webSocketLog_, msg);
        }
        else if( jsonObject.getString("type").equals("message") ){
            String message = userName + ": " +  jsonObject.getString("message");
            msg = message;
            Log.d(webSocketLog_, msg);



        }
        else if ( jsonObject.getString("type").equals("leave") ){
            msg = userName + " has left the room";
            Log.d(webSocketLog_, msg);
        }
        ChatRoom.messages.add(msg);
        ChatRoom.listView_.post(new Runnable() {
            @Override
            public void run() {


        ChatRoom.myAdapter_.notifyDataSetChanged();
        ChatRoom.listView_.smoothScrollToPosition(ChatRoom.myAdapter_.getCount());
            }
        });
    }

}//END OF CLASS

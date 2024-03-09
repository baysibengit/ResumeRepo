package com.example.androidchatclient;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Adapter;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;

import com.neovisionaries.ws.client.WebSocket;
import com.neovisionaries.ws.client.WebSocketFactory;

import java.io.IOException;
import java.util.ArrayList;

public class ChatRoom extends AppCompatActivity {

    public static String roomName_;
    public static String userName_;
    public TextView currentRoom_;
    public EditText messageBox_;
    public String message_;
    final String roomUserNames_ = "ChatRoom:Bb";
    public static ListView listView_;
    public static ArrayList<String> messages = new ArrayList<>();

    public static ArrayAdapter myAdapter_;
    private static final String WS_URL = "ws://10.0.2.2:8080/endpoint";
    public static WebSocket ws_;


    //WebSocket ws = null;
    //ws_ = null;

    @SuppressLint("WrongViewCast")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_chat_room);

        //Get data from activity
        Bundle extras = getIntent().getExtras();

        if( extras != null ){
            //Extract roomName and userName
            roomName_ = extras.getString("Room name");
            userName_ = extras.getString("User name");

            Log.d(roomUserNames_, "Room: " + roomName_);
            Log.d(roomUserNames_, "Username: " + userName_);

            //Set text view to current room name
            currentRoom_ = findViewById(R.id.currentRoom);
            currentRoom_.setText(roomName_);
            Log.d(roomUserNames_, "Current Room: " + roomName_);

            //Grabbing the listview
            listView_ = findViewById(R.id.messageHistory);

            //Create array adapters
            myAdapter_ = new ArrayAdapter(this, android.R.layout.simple_list_item_1, messages);

            listView_.setAdapter(myAdapter_);
        }


        try {
            Log.d( "Bb", "about to create a WS");
            ws_ = new WebSocketFactory().createSocket(WS_URL);
            //listen for event and will use this class to implement them
            ws_.addListener(new MyWebSocket());
            ws_.connectAsynchronously();

        } catch (IOException e) {
            //AlertDialog alert = new AlertDialog("Server failed");
            Log.d("chatactivity", "some error");
        }
        Log.d( "Bb", "done with oncreate");

    }

    public static void sendJoin() {
        // send "join <username> <roomname>" to server
        ws_.sendText(" {\"type\":\"join\",\"user\":\""+userName_+"\",\"room\":\""+roomName_+"\"}");
    }

    public void handleSendMsg(View view ) {
        // if wsIsOpen == false ... let user know and/or just return
        if(!MyWebSocket.wsIsOpen){
            displayAlert("WebSocket must be open ");
        }
        else {
            Log.d(roomUserNames_, "hello");
            EditText messageBox = findViewById(R.id.messageBox);
            String message = messageBox.getText().toString();
            Log.d("Sent Message", "Message sent: " + message);
            // message => Hello how are you?
            ws_.sendText(" {\"type\":\"message\",\"user\":\"" + userName_ + "\",\"room\":\"" + roomName_ + "\",\"message\":\"" + message + "\"}");
        }
    }

    public void handleLeaveMsg(View view){
        ws_.sendText(" {\"type\":\"leave\",\"user\":\""+userName_+"\",\"room\":\""+roomName_+"\"}");
        MyWebSocket.wsIsOpen = false;
        Intent leaveIntent = new Intent( ChatRoom.this, MainActivity.class );
        startActivity(leaveIntent);

    }

    private void displayAlert( String alertMessage ) {

        AlertDialog.Builder builder = new AlertDialog.Builder( this );
        builder.setMessage( alertMessage );
        AlertDialog alertDialog = builder.create();
        alertDialog.show();

    }
}//END OF CLASS
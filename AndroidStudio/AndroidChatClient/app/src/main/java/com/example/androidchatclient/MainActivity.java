package com.example.androidchatclient;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;

public class MainActivity extends AppCompatActivity {

    //Member variables
    final String logKey = "MainActivity:Bb";
    //public String roomName_ = "";
    //public String userName_ = "";
    public EditText userNameInput_;
    public EditText roomNameInput_;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Log.d( logKey, "Main Activity has been created...");
    }

    public void handleJoin( View view ) {

        Log.d(logKey, "Join button was pressed");

        //Grab object Ids and assigning them to edit text objects
        userNameInput_ = findViewById(R.id.userName);
        roomNameInput_ = findViewById(R.id.roomName);

        //Transforming and assigning to string
        String roomName = (roomNameInput_.getText().toString());
        String userName = (userNameInput_.getText().toString());

        //Create an intent
        Intent intent = new Intent( MainActivity.this, ChatRoom.class );
        //Store room and username fields in intent object
        intent.putExtra("Room name", roomName);
        intent.putExtra("User name", userName);
        //Start activity
        startActivity(intent);
    }
}
package com.examples.ledotron

import android.content.Intent
import android.os.Bundle
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat


class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setContentView(R.layout.inicio)
        //accion de boton para abrir otra ventana

        val launchButton = findViewById<Button>(R.id.iniciar)
        launchButton.setOnClickListener{
            val intent = Intent(this, opcionbluetooth::class.java)
            startActivity(intent)
        }

    }


}
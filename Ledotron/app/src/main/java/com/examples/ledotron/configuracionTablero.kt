package com.examples.ledotron

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Bundle
import android.view.View
import android.widget.AdapterView
import android.widget.Button
import android.widget.EditText
import android.widget.SeekBar
import android.widget.Spinner
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import java.util.UUID


class configuracionTablero() : AppCompatActivity() {
    lateinit var BTS: BluetoothSocket




    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)


        setContentView(R.layout.configuraciontablero)



        val bluetoothManager = getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager


        val bta = bluetoothManager.adapter as BluetoothAdapter
        val btnEna =  Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
        PedirPermisoVincular(intent.getStringExtra("valor").toString(),bta)



        val rangoT = findViewById<SeekBar>(R.id.TransicionSB)
        val valorTransicion = findViewById<TextView>(R.id.TransicionNumero)
        valorTransicion.text = rangoT.progress.toString()




        val rangoV = findViewById<SeekBar>(R.id.VelocidadSB)
        val valorVelocidad = findViewById<TextView>(R.id.velocidadNumero)
        valorVelocidad.text = rangoV.progress.toString()

        val BotonEstilo = findViewById<Button>(R.id.estiloBoton)
        val BotonReset = findViewById<Button>(R.id.resetBoton)
        val BotonGuardado = findViewById<Button>(R.id.guardarBoton)
        val BotonPredeterminado = findViewById<Button>(R.id.predeterminadoBoton)
        val BotonMensaje = findViewById<Button>(R.id.VisualizarBoton)
        val BotonmasBrillo = findViewById<Button>(R.id.brilloBoton)
        val listaJustificado = findViewById<Spinner>(R.id.listajustificado)




        val mensaje = findViewById<EditText>(R.id.mensaje)

        BotonReset.setOnClickListener {
            BTS.outputStream.write(("*R~").toByteArray());
        }
        BotonGuardado.setOnClickListener {
            BTS.outputStream.write(("*W~").toByteArray());
        }
        BotonMensaje.setOnClickListener {
            BTS.outputStream.write(("*M"+mensaje.text+"~").toByteArray());
        }
        BotonPredeterminado.setOnClickListener {
            BTS.outputStream.write(("*F~").toByteArray());
        }
        BotonEstilo.setOnClickListener {
            BTS.outputStream.write(("*I~").toByteArray());
        }

        BotonmasBrillo.setOnClickListener {
            BTS.outputStream.write(("*B1~").toByteArray());
        }




        listaJustificado.setOnItemSelectedListener(object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(
                parentView: AdapterView<*>?,
                selectedItemView: View?,
                position: Int,
                id: Long
            ) {

            }

            override fun onNothingSelected(parentView: AdapterView<*>?) {
                // your code here
            }
        })

        rangoV.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener{ // velocidad

            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {

            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                if (seekBar != null) {
                    valorVelocidad.text = ((seekBar.progress*3000)/100).toString()
                    val num:Int = (seekBar.progress*3000)/100;
                    BTS.outputStream.write(("*P"+(num).toString()+"~").toByteArray());
                }
            }
        })

        rangoT.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener{ // Transicion

            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {

            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                if (seekBar != null) {
                    valorTransicion.text = ((seekBar.progress*3000)/100).toString()
                    val num:Int = (seekBar.progress*3000)/100;

                    BTS.outputStream.write(("*S"+((num).toString()+"~")).toByteArray());
                }
            }
        })


    }

    private fun PedirPermisoVincular(DireccionesBT: String, bta: BluetoothAdapter) {
        if(ContextCompat.checkSelfPermission(this,
                Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED){
            // permiso no aceptado
            requestBT()
        }else{
            //permiso aceptado
            if(!bta.bondedDevices.isEmpty()){
                try {
                    val ui: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
                    //val BTS:BluetoothSocket

                    val instalacion: BluetoothDevice = bta.getRemoteDevice(DireccionesBT)
                    BTS =  instalacion.createRfcommSocketToServiceRecord(ui)
                    BTS.connect()
                    Toast.makeText(this,"se ha podido conectar",Toast.LENGTH_SHORT).show()
                }catch (e:Exception){
                    Toast.makeText(this,"no se ha podido conectar",Toast.LENGTH_SHORT).show()

                }

            }
        }
    }

    private fun requestBT() {
        if(ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.BLUETOOTH_CONNECT)){
            // el usuario rechazo el bluetooth\
            Toast.makeText(this,"Permiso rechazado",Toast.LENGTH_SHORT).show()
        }else{
            // pedir permiso
            ActivityCompat.requestPermissions(this,
                arrayOf( Manifest.permission.BLUETOOTH_CONNECT),777)
        }
    }


}



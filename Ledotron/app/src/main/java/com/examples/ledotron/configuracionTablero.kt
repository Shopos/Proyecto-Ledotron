package com.examples.ledotron

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.graphics.Color
import android.graphics.drawable.Drawable
import android.os.Bundle
import android.provider.CalendarContract.Colors
import android.util.Log
import android.view.View
import android.widget.AdapterView
import android.widget.ArrayAdapter
import android.widget.Button
import android.widget.EditText
import android.widget.SeekBar
import android.widget.Spinner
import android.widget.TableLayout
import android.widget.TableRow
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import java.util.Date
import java.util.Timer
import java.util.TimerTask
import java.util.UUID


class configuracionTablero() : AppCompatActivity() {

    private val letras = mapOf(
        'A' to arrayOf(
            " *** ",
            " * * ",
            " * * ",
            " *** ",
            " * * ",
            " * * ",
            " * * "
        ),
        'B' to arrayOf(
            " *** ",
            " * * ",
            " * * ",
            " *** ",
            " * * ",
            " * * ",
            " *** "
        ),
        'C' to arrayOf(
            " *** ",
            " *   ",
            " *   ",
            " *   ",
            " *   ",
            " *   ",
            " *** "
        ),
        'D' to arrayOf(
            " **  ",
            " * * ",
            " * * ",
            " * * ",
            " * * ",
            " * * ",
            " **  "
        ),
        'E' to arrayOf(
            " *** ",
            " *   ",
            " *   ",
            " *** ",
            " *   ",
            " *   ",
            " *** "
        ),
        'F' to arrayOf(
            " *** ",
            " *   ",
            " *   ",
            " *** ",
            " *   ",
            " *   ",
            " *   "
        ),
        'G' to arrayOf(
            " *** ",
            " *   ",
            " *   ",
            " * **",
            " * * ",
            " * * ",
            " *** "
        ),
        'H' to arrayOf(
            " * * ",
            " * * ",
            " * * ",
            " *** ",
            " * * ",
            " * * ",
            " * * "
        ),
        'I' to arrayOf(
            " *** ",
            "  *  ",
            "  *  ",
            "  *  ",
            "  *  ",
            "  *  ",
            " *** "
        ),
        'J' to arrayOf(
            " *** ",
            "   * ",
            "   * ",
            "   * ",
            "   * ",
            " * * ",
            " *** "
        ),
        'K' to arrayOf(
            " * * ",
            " * * ",
            " * * ",
            " **  ",
            " * * ",
            " * * ",
            " * * "
        ),
        'L' to arrayOf(
            " *   ",
            " *   ",
            " *   ",
            " *   ",
            " *   ",
            " *   ",
            " *** "
        ),
        'M' to arrayOf(
            " * * ",
            " *** ",
            " *** ",
            " * * ",
            " * * ",
            " * * ",
            " * * "
        ),
        'N' to arrayOf(
            " * * ",
            " *** ",
            " *** ",
            " *** ",
            " * * ",
            " * * ",
            " * * "
        ),
        'O' to arrayOf(
            " *** ",
            " * * ",
            " * * ",
            " * * ",
            " * * ",
            " * * ",
            " *** "
        ),
        'P' to arrayOf(
            " *** ",
            " * * ",
            " * * ",
            " *** ",
            " *   ",
            " *   ",
            " *   "
        ),
        'Q' to arrayOf(
            " *** ",
            " * * ",
            " * * ",
            " * * ",
            " * * ",
            " **  ",
            " ****"
        ),
        'R' to arrayOf(
            " *** ",
            " * * ",
            " * * ",
            " *** ",
            " **  ",
            " * * ",
            " * * "
        ),
        'S' to arrayOf(
            " *** ",
            " *   ",
            " *   ",
            " *** ",
            "   * ",
            "   * ",
            " *** "
        ),
        'T' to arrayOf(
            " *** ",
            "  *  ",
            "  *  ",
            "  *  ",
            "  *  ",
            "  *  ",
            "  *  "
        ),
        'U' to arrayOf(
            " * * ",
            " * * ",
            " * * ",
            " * * ",
            " * * ",
            " * * ",
            " *** "
        ),
        'V' to arrayOf(
            " * * ",
            " * * ",
            " * * ",
            " * * ",
            " * * ",
            " * * ",
            "  *  "
        ),
        'W' to arrayOf(
            " * * ",
            " * * ",
            " * * ",
            " * * ",
            " *** ",
            " *** ",
            " * * "
        ),
        'X' to arrayOf(
            " * * ",
            " * * ",
            " * * ",
            "  *  ",
            " * * ",
            " * * ",
            " * * "
        ),
        'Y' to arrayOf(
            " * * ",
            " * * ",
            " * * ",
            "  *  ",
            "  *  ",
            "  *  ",
            "  *  "
        ),
        'Z' to arrayOf(
            " *** ",
            "   * ",
            "  *  ",
            "  *  ",
            " *   ",
            " *   ",
            " *** "
        )
    )

    var iniciocolumna = 0
    var iniciofila = 0
    var iniciofilalocal = iniciofila
    var iniciocolumnalocal = iniciocolumna

    var  fusionmatriz = letras['H']



    lateinit var BTS: BluetoothSocket
    var palabra = "HOLA"



    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)





        setContentView(R.layout.configuraciontablero)



        val bluetoothManager = getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager


        val bta = bluetoothManager.adapter as BluetoothAdapter
        val btnEna =  Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
        //PedirPermisoVincular(intent.getStringExtra("valor").toString(),bta)



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
        val BotonJustificado = findViewById<Button>(R.id.justificadoBoton)
        val BotonAtras = findViewById<Button>(R.id.atras)
        val tablerosim = findViewById<TableLayout>(R.id.tablerosimulacion)



/*

        val timer = Timer()
        // Crear un TimerTask
        val task = object : TimerTask() {
            override fun run() {

                var iniciofilalocal = iniciofila
                var iniciocolumnalocal = iniciocolumna


                if (iniciofilalocal < tablerosim.childCount) {

                    // Recorrer columnas en la fila actual
                }else{
                    iniciofilalocal = iniciofila
                }
                val row = tablerosim.getChildAt(iniciofilalocal) as TableRow

                if (iniciocolumnalocal < row.childCount) {



                }else{
                    iniciofilalocal++
                    iniciocolumnalocal = iniciocolumna
                }
                val cell = row.getChildAt(iniciocolumnalocal) as TextView

                cell.setBackgroundColor(Color.parseColor("#000000"))

                println("$iniciofilalocal $iniciocolumnalocal")
                iniciocolumnalocal++

            }
        }
        timer.schedule(task, 0, 100)
*/

        val timer = Timer()
        // Crear un TimerTask
        val task = object : TimerTask() {
            override fun run() {



                if((fusionmatriz?.size ?:  0) != 0){
                    for (i in 0 until tablerosim.childCount) {
                        val row = tablerosim.getChildAt(i) as TableRow
                        // Recorrer columnas en la fila actual
                        for (j in 0 until row.childCount) {
                            val cell = row.getChildAt(j) as TextView
                            cell.setBackgroundColor(Color.parseColor("#000000"))

                        }
                    }

                    val aste = fusionmatriz
                    if (aste != null) {
                        var j = iniciofilalocal
                        var h = iniciofila
                        while((j  < tablerosim.childCount) && h < aste.size){
                            println(aste[h])
                            val row = tablerosim.getChildAt(j) as TableRow
                            var z = iniciocolumna
                            if(iniciocolumnalocal >= row.childCount){
                                iniciocolumnalocal = 0
                            }
                            var i = iniciocolumnalocal

                            while((z  < row.childCount) && i < aste[h].length){
                                val cell = row.getChildAt(z) as TextView
                                if(aste[h][i] == '*'){
                                    cell.setBackgroundColor(Color.parseColor("#ff0000"))
                                }
                                z++
                                i++
                            }
                            j++
                            h++
                        }
                    }
                    iniciocolumnalocal++


                }
            }
        }
        timer.schedule(task, 0, 100)


        BotonAtras.setOnClickListener {
            val intent = Intent(this, opcionbluetooth::class.java)
            startActivity(intent)

        }





        val mensaje = findViewById<EditText>(R.id.mensaje)

        BotonJustificado.setOnClickListener {
                BTS.outputStream.write(("*J~").toByteArray());

        }
        BotonReset.setOnClickListener {
            //BTS.outputStream.write(("*R~").toByteArray());
        }
        BotonGuardado.setOnClickListener {
            BTS.outputStream.write(("*W~").toByteArray());
        }
        BotonMensaje.setOnClickListener {
            //BTS.outputStream.write(("*M"+mensaje.text+"~").toByteArray());

            iniciofilalocal = iniciofila
            iniciocolumnalocal = iniciocolumna

            for (i in 0 until tablerosim.childCount) {
                val row = tablerosim.getChildAt(i) as TableRow
                // Recorrer columnas en la fila actual
                for (j in 0 until row.childCount) {
                    val cell = row.getChildAt(j) as TextView
                    cell.setBackgroundColor(Color.parseColor("#000000"))

                }
            }



            var palabra = mensaje.text.toString().uppercase()

            fusionmatriz = letras[palabra[0]]?.clone()
            val palabrasub = palabra.substring(1)
            if (fusionmatriz != null) {

                for(letracola in palabrasub){
                    val filas = letras[letracola]?.clone()
                    if (filas != null) {
                        for(j in filas.indices){
                            fusionmatriz!![j] += filas[j]
                        }
                    }

                }
            }

            val aste = fusionmatriz
            if (aste != null) {
                var j = iniciofilalocal
                var h = 0
                while((j  < tablerosim.childCount) && h < aste.size){
                    println(aste[h])
                    val row = tablerosim.getChildAt(j) as TableRow
                    var z = iniciocolumnalocal
                    var i = 0
                    while((z  < row.childCount) && i < aste[h].length){
                        val cell = row.getChildAt(z) as TextView
                        if(aste[h][i] == '*'){
                            cell.setBackgroundColor(Color.parseColor("#ff0000"))
                        }
                        z++
                        i++
                    }
                    j++
                    h++
                }
            }

            /**
            val iniciofilalocal = iniciofila
            val iniciocolumnalocal = iniciocolumna

            println(letras['A']?.get(0))
            val aste = letras['A']

            if (aste != null) {

                var j = iniciofilalocal
                var h = 0
                while((j  < tablerosim.childCount) && h < aste.size){
                    println(aste[h])
                    val row = tablerosim.getChildAt(j) as TableRow
                    var z = iniciocolumnalocal
                    var i = 0
                    while((z  < row.childCount) && i < aste[h].length){
                        val cell = row.getChildAt(z) as TextView
                        val lea = aste[h][i]
                        if( lea == '*'){
                            cell.setBackgroundColor(Color.parseColor("#ff0000"))
                        }
                        z++
                        i++
                    }
                    j++
                    h++
                }
            }*/
            /**
            if (iniciofila < tablerosim.childCount) {

                // Recorrer columnas en la fila actual
            }else{
                iniciofila = 0
            }
            var row = tablerosim.getChildAt(iniciofila) as TableRow

            if (iniciocolumna < row.childCount) {



            }else{
                iniciofila++
                iniciocolumna = 0
            }

            row = tablerosim.getChildAt(iniciofila) as TableRow
            val cell = row.getChildAt(iniciocolumna) as TextView

            cell.setBackgroundColor(Color.parseColor("#000000"))

            println("$iniciofila $iniciocolumna")
            iniciocolumna++*/
/**
            for (i in iniciofila until tablerosim.childCount) {
                val row = tablerosim.getChildAt(i) as TableRow
                // Recorrer columnas en la fila actual
                for (j in iniciocolumna until row.childCount) {
                    val cell = row.getChildAt(j) as TextView
                    cell.setBackgroundColor(Color.parseColor("#000000"))

                }
            }*/

        }
        BotonPredeterminado.setOnClickListener {
            BTS.outputStream.write(("*F~").toByteArray());
        }


        BotonEstilo.setOnClickListener {
            BTS.outputStream.write(("*I~").toByteArray()); // *H~
        }

        BotonmasBrillo.setOnClickListener {
            BTS.outputStream.write(("*B1~").toByteArray());
        }



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
                    valorTransicion.text = ((seekBar.progress*500)/100).toString()
                    val num:Int = (seekBar.progress*500)/100;

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



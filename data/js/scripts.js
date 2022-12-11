// Empty JS for your own code to be here

function domReady(f) {
  if (document.readyState === 'complete') {
    f();
  } else {
    document.addEventListener('DOMContentLoaded', f);
  }
}

function connectWebSocket()
{
    if (connection.readyState == 3)
    {
        connection.connection();
        }
}

function hexToDec(hex) {
    var result = 0, digitValue;
    hex = hex.toLowerCase();
    for (var i = 0; i < hex.length; i++)
    {
        if (hex[i] != '#')
        {
            digitValue = '0123456789abcdefgh'.indexOf(hex[i]);
            result = result * 16 + digitValue;
        }
    }
    return result;
}

domReady(function () {
    var idTimer;
    var connection = new WebSocket('ws://' + location.hostname + '/ws');
    //    var connection = new WebSocket('ws://' + location.hostname + ':81');

    var el = document.getElementById(btInfoConnection);
    if (el != null)
    {
        el.onclick = connectWebSocket;
    }

    function onTimerWS()
    {
      switch(connection.readyState)
      {
        case 0:
              document.getElementById("btInfoConnection").innerHTML = "en connexion";
              document.getElementById("btInfoConnection").setAttribute("title", "socket créé mais la connexion n'est pas encore ouverte");          
          break;
        case 1:
              document.getElementById("btInfoConnection").innerHTML = "connecté";
              document.getElementById("btInfoConnection").setAttribute("title", "Connexion ouverte et prête à communiquer");          
          break;
        case 2:
              document.getElementById("btInfoConnection").innerHTML = "fermeture en cours";
              document.getElementById("btInfoConnection").setAttribute("title", "Connexion encore ouverte, mais en cours de fermeture");          
          break;
        case 3:
              document.getElementById("btInfoConnection").innerHTML = "déconnecté";
              document.getElementById("btInfoConnection").setAttribute("title", "Connexion fermée ou ne pouvant être ouverte");          
          break;
      };
    }
    
    connection.onopen = function (evt)
    {
        console.log('WebSocket ouverture', evt);
        connection.send('{\"Connect\":\"' + new Date() + '\"}');
        document.getElementById("btInfoConnection").classList.remove("btn-danger");
        document.getElementById("btInfoConnection").classList.add("btn-success");
        document.getElementById("IPserveurWS").innerHTML = location.hostname;
        idTimer = setInterval(onTimerWS, 3000);
    };

    connection.onerror = function (error)
    {
        console.log('WebSocket Error ', error);
    };

    connection.onclose = function (evt)
    {
        console.log('WebSocket fermeture ', evt);
        document.getElementById("btInfoConnection").classList.remove("btn-success");
        document.getElementById("btInfoConnection").classList.add("btn-danger");
        clearInterval(idTimer);
        onTimerWS();
    };

    connection.onmessage = function (event)
    {
        console.log('WebSocket : message', event);
        document.getElementById("logText").innerHTML += event.data + "<br/>";

        try
        {
            var msg = JSON.parse(event.data);

            Object.keys(msg).forEach(function (key)
            {
                switch (key)
                {
                    case "val_temp":
                        document.getElementById(key).innerHTML = msg[key].toFixed(1);
                        break;
                    case "val_hygro":
                        document.getElementById(key).innerHTML = msg[key].toFixed(0);
                        break;
                    case "val_hygroAbs":
                        document.getElementById(key).innerHTML = msg[key];
                        break;
                    case "val_pression":
                        document.getElementById(key).innerHTML = msg[key].toFixed(0);
                        break;
                    case "val_eco2":
                        document.getElementById(key).innerHTML = msg[key];
                        break;
                    case "val_tcov":
                        document.getElementById(key).innerHTML = msg[key];
                        break;
                    case "val_batt":
                        document.getElementById(key).innerHTML = msg[key];
                        break;
                    case "val_battVolt":
                        document.getElementById(key).innerHTML = msg[key].toFixed(2);
                        break;
                }
             });
        }
        catch (e)
        {
            console.error("Parsing error:", e);
            console.log(event.data);
        }
    };
});  // fin domReady

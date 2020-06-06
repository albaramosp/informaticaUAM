
function submitSelection(id) {
  document.getElementById(id).submit();
}

function hide(id){
  el = document.getElementById(id);
  el.style.visibility = 'hidden';
  el.style.display = 'block';
}

function showAlert(message) {
  alert(message);
}

function calculateTotal(){
   var total = 12.0;

  $("#carritoTable tr:gt(0) td:nth-child(5)").each(function(){
    var t=$(this).html();
    total += parseFloat(String(t).substring(0, t.length - 1)).toFixed(2);
 });

 alert('calculando total ' + total);
  return total
}

function deleteSelected(){
  var selected = [];
  $("#carritoTable  input:checked").each(function(){
        var t=$(this).parent().find("#itemId").val() ;
        selected.push(t);
    });

  if ( selected.length < 1 ){
      alert("No ha seleccionado ningun articulo");
    }
  else {
      $('#cartAction').val('delete');
      $('#selectedItems').val(selected);
      $('#cartButtons').submit();
    }
}

function startTimer(url){
  setInterval(actualizarContador.bind(window, url), 3000);
}

function actualizarContador(url){
  //Creamos objeto XHTTP
  var xhttp = new XMLHttpRequest();

  //Recogemos respuesta y estado del servidor en callback
  xhttp.onreadystatechange = function (){
    if (xhttp.readyState == 4 && xhttp.status == 200){
      document.getElementById("numUsuarios").innerHTML = " " + xhttp.responseText;
    }
  }

  //Enviamos la peticion al servidor
  xhttp.open("GET", url, true);
  xhttp.send();
}

function getServerData(input, url){
  //Creamos objeto XHTTP
  var xhttp = new XMLHttpRequest();

  //Recogemos respuesta y estado del servidor en callback
  xhttp.onreadystatechange = function (){
    if (xhttp.readyState == 4 && xhttp.status == 200){
      document.getElementById(input).value = xhttp.responseText;
    }
  }

  //Enviamos la peticion al servidor
  xhttp.open("GET", url, true);
  xhttp.send();
  
}

function checkPasswordCoincidence(input) {
    if (input.value != document.getElementById('password').value) {
        input.setCustomValidity('Las contraseñas no coinciden');
    } else {
        // input is valid -- reset the error message
        input.setCustomValidity('');
    }
}

function controlTogle(id){
  id = 'tr#'+id;
  $(id).toggle();
  }

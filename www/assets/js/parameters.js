function clone(obj) {
  var copy;

  // Handle the 3 simple types, and null or undefined
  if (null == obj || "object" != typeof obj) return obj;

  // Handle Date
  if (obj instanceof Date) {
    copy = new Date();
    copy.setTime(obj.getTime());
    return copy;
  }

  // Handle Array
  if (obj instanceof Array) {
    copy = [];
    for (var i = 0, len = obj.length; i < len; i++) {
      copy[i] = clone(obj[i]);
    }
    return copy;
  }

  // Handle Object
  if (obj instanceof Object) {
    copy = {};
    for (var attr in obj) {
      if (obj.hasOwnProperty(attr)) copy[attr] = clone(obj[attr]);
    }
    return copy;
  }

  throw new Error("Unable to copy obj! Its type isn't supported.");
}

angular.module("ParameterApp", ['ui-notification']).controller("ParameterCtrl", function($scope, $http, Notification) {
  $scope.visibleState = false;
  $scope.parameters = {
    regulator: {
      Kp: 0.0,
      Ki: 0.0,
      min_istate: 0.0,
      max_istate: 0.0
    }
  };
  $scope.parameters_bck = {};


  //Callback for send button
  $scope.sendParameters = function() {
    Notification.success({
      message: "Parameters written to server"
    });
  };

  //Callback for reset button
  $scope.resetParameters = function() {
    $scope.parameters = clone($scope.parameters_bck);
  };

  //Fetch parameters from server on load
  window.onload = function() {
    $http.get("get_parameters").then(function successCallback(response) {
      $scope.visibleState = true;
      $scope.parameters = response.data;
      $scope.parameters_bck = clone($scope.parameters);
    }, function errorCallback(response) {
      Notification.error({
        message: "Failed to receive parameters from server."
      });
    });
  }

}).config(function(NotificationProvider) {
  NotificationProvider.setOptions({
    delay: 10000,
    startTop: 20,
    startRight: 10,
    verticalSpacing: 20,
    horizontalSpacing: 20,
    positionX: 'left',
    positionY: 'bottom'
  });
});

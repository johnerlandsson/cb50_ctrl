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

angular.module("ParameterApp", []).controller("ParameterCtrl", function($scope, $http) {
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
    console.log("Send");
  };

  //Callback for reset button
  $scope.resetParameters = function() {
    console.log($scope.parameters['regulator']);
    $scope.parameters = clone($scope.parameters_bck);
    //$scope.$apply();
  };

  $http.get("get_parameters").then(function successCallback(response) {
    $scope.visibleState = true;
    $scope.parameters = response.data;
    $scope.parameters_bck = clone($scope.parameters);
  }, function errorCallback(response) {
    console.log(response);
  });
});

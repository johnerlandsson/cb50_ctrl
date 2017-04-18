/* 
 * This file is part of the cb50_ctrl distribution (https://github.com/johnerlandsson/cb50_ctrl).
 * Copyright (c) 2017 John Erlandsson
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

//The module
angular.module("RecipeApp", ['ui-notification']).controller("RecipeCtrl", function($scope, $http, Notification) {
  $scope.pd = {};
  $scope.recipeBrowserVisible = false;
  $scope.recipeViewerVisible = false;
  $scope.recipe_names = [];
  $scope.current_recipe = {};
  $scope.editRecipeEntryVisible = false;
  $scope.entryToBeEdited = {};
  $scope.entryToBeEditedIndex = 0;
  $scope.selectedRecipeName = '';
	$scope.addingNewRecipe = false;
	$scope.newRecipeName = "";

	$scope.confirmAddNewRecipe = function() {
		console.log("here");
		if($scope.newRecipeName.length <= 0) {
      Notification.error({
				message: "Error: Can't add recipe without name"
      });
			return;
		}

		$scope.recipe_names.push($scope.newRecipeName);
		$scope.selectedRecipeName = $scope.newRecipeName;
		$scope.current_recipe = {name: $scope.newRecipeName, entries: [{sv: 100.0, time: 60, confirm: false}]};
		$scope.addingNewRecipe = false;
	}

	$scope.cancelAddNewRecipe = function() {
		$scope.newRecipeName = "";
		$scope.addingNewRecipe = false;
	}


	$scope.raise_entry = function(index) {
		if(index <= 0) return;
		var tmp = $scope.current_recipe['entries'][index];
		$scope.current_recipe['entries'][index] = $scope.current_recipe['entries'][index - 1];
		$scope.current_recipe['entries'][index - 1] = tmp;
		syncRecipe();
	}

	$scope.lower_entry = function(index) {
		if(index >= $scope.current_recipe['entries'].length - 1) return;
		var tmp = $scope.current_recipe['entries'][index];
		$scope.current_recipe['entries'][index] = $scope.current_recipe['entries'][index + 1];
		$scope.current_recipe['entries'][index + 1] = tmp;
		syncRecipe();
	}

  $scope.remove_recipe_entry = function(index) {
    $scope.entryToBeEditedIndex = 0;

		if($scope.current_recipe['entries'].length <= 1) {
      Notification.error({
				message: "Error: Recipe needs at least one entry"
      });
			return;
		}

    if (index >= 0) $scope.current_recipe['entries'].splice(index, 1);
		syncRecipe();
  }

  $scope.addRecipe = function() {
		$scope.addingNewRecipe = true;
  }

  $scope.removeRecipe = function() {
    console.log("removeRecipe");
  }

  $scope.selectedRecipeChanged = function() {
    console.log($scope.selectedRecipeName);
  }

  function syncRecipe() {
    $http.post('sync_recipe', {
      message: $scope.current_recipe
    }).then(function successCallback(response) {
      Notification.success({
        message: "Successfully synced recipe"
      });
      $scope.editRecipeEntryVisible = false;
    }, function errorCallback(response) {
      Notification.error({
        message: "Failed to sync recipe: " + response.body
      });
    });
  }

  $scope.cancelEditing = function() {
    $scope.editRecipeEntryVisible = false;
  }

  $scope.editingFinished = function() {
    $scope.current_recipe['entries'][$scope.entryToBeEditedIndex] = $scope.entryToBeEdited;
    syncRecipe();
  }

  $scope.edit_recipe_entry = function(index) {
    $scope.entryToBeEditedIndex = index;
    $scope.entryToBeEdited = $scope.current_recipe['entries'][index];
    $scope.editRecipeEntryVisible = true;
  }

  // Is called after successful get_recipe()
  function current_recipe_updated() {
    //console.log($scope.current_recipe['name']);
    $scope.selectedRecipeName = $scope.current_recipe['name'];
    $scope.entryToBeEdited = $scope.current_recipe['entries'][0];
  }

  function get_recipe(name) {
    $http.get("get_recipe/" + name).then(function successCallback(response) {
      $scope.current_recipe = response.data;
      current_recipe_updated();
    }, function errorCallback(response) {
      Notification.error({
        message: "Failed to receive recipe: " + name + " from server."
      });
    });
  }

  // Fetch array of recipe names from server
  function get_recipe_names() {
    $http.get("get_recipe_names").then(function successCallback(response) {
      $scope.recipe_names = response.data;
    }, function errorCallback(response) {
      Notification.error({
        message: "Failed to receive recipe names from server."
      });
    });
  }

  // Fetch process data from server
  function get_pd() {
    $http.get("get_pd").then(function successCallback(response) {
      $scope.pd = response.data;
    }, function errorCallback(response) {
      Notification.error({
        message: "Failed to receive process data from server."
      });
    });
  }

  function set_pump_button_bg() {
    if ($scope.pd["pump"]) {
      $scope.pump_btn_style = {
        "background-color": "rgb(102, 102, 102)"
      };
    } else {
      $scope.pump_btn_style = {
        "background-color": "rgb(153, 153, 153)"
      };
    }
  }

  function set_mixer_button_bg() {
    if ($scope.pd["mixer"]) {
      $scope.mixer_btn_style = {
        "background-color": "rgb(102, 102, 102)"
      };
    } else {
      $scope.mixer_btn_style = {
        "background-color": "rgb(153, 153, 153)"
      };
    }
  }

  function set_run_button_bg() {
    if ($scope.pd["run_recipe"]) {
      $scope.run_btn_style = {
        "background-color": "rgb(102, 102, 102)"
      };
    } else {
      $scope.run_btn_style = {
        "background-color": "rgb(153, 153, 153)"
      };
    }
  }

  // Fetch process data from server and update view accordingly
  function update_pd() {
    get_pd();

    // Set button colors
    set_pump_button_bg();
    set_mixer_button_bg();
    set_run_button_bg();

    $scope.recipeViewerVisible = $scope.pd["run_recipe"];
    $scope.recipeBrowserVisible = !$scope.recipeViewerVisible;
  }


  // Pump button clicked
  $scope.pump_btn_clicked = function() {
    window.clearInterval($scope.intervalID);
    $scope.pd["pump"] = !$scope.pd["pump"];
    set_pump_button_bg();
    $http.post('put_pd', {
      message: $scope.pd
    }).then(function successCallback(response) {
      $scope.pd = response.data;
      $scope.intervalID = window.setInterval(update_pd, 500);
    }, function errorCallback(response) {
      Notification.error({
        message: "Failed send pump value"
      });
    });
  }

  // Run recipe button clicked
  $scope.run_btn_clicked = function() {
    window.clearInterval($scope.intervalID);
    $scope.pd["run_recipe"] = !$scope.pd["run_recipe"];
    $scope.recipeViewerVisible = $scope.pd["run_recipe"];
    $scope.recipeBrowserVisible = !$scope.recipeViewerVisible;
    set_run_button_bg();
    $http.post('put_pd', {
      message: $scope.pd
    }).then(function successCallback(response) {
      $scope.pd = response.data;
      $scope.intervalID = window.setInterval(update_pd, 500);
    }, function errorCallback(response) {
      Notification.error({
        message: "Failed send run recipe value"
      });
    });
  }

  // Mixer button clicked
  $scope.mixer_btn_clicked = function() {
    window.clearInterval($scope.intervalID);
    $scope.pd["mixer"] = !$scope.pd["mixer"];
    set_mixer_button_bg();
    $http.post('put_pd', {
      message: $scope.pd
    }).then(function successCallback(response) {
      $scope.pd = response.data;
      $scope.intervalID = window.setInterval(update_pd, 500);
    }, function errorCallback(response) {
      Notification.error({
        message: "Failed send mixer value"
      });
    });
  }

  $scope.intervalID = window.setInterval(update_pd, 500);

  window.onload = function() {
    $http.get("get_recipe_names").then(function successCallback(response) {
      $scope.recipe_names = response.data;
      get_recipe($scope.recipe_names[0]);
    }, function errorCallback(response) {
      Notification.error({
        message: "Failed to receive recipe names from server."
      });
    });
  }

}).config(function(NotificationProvider) { //Configure notifications
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

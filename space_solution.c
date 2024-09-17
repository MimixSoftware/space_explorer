#include "space_explorer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <float.h>

// My solution. space_explorer.c and space_explorer.h were given and are used to generate the universe and run the game.

typedef struct RoutePoint
{
	unsigned int planet_id; //ID of planet
	bool visited; //Was this planet visited?
	double distance_from_mixer; //Distance from mixer (treasure)
	struct RoutePoint **connections; //A list of connections
	unsigned int num_connections; //Number of connections
	struct RoutePoint *next; //Next routepoint in route
} RoutePoint;


typedef struct
{
	RoutePoint* first;
	RoutePoint* last;
	unsigned int size;
} Route; //A LinkedList to store visited planets

typedef struct
{
	Route *route; //Visited planets
	RoutePoint *current; // //The current planet
	RoutePoint *next; //The next planet
	unsigned int num_hops; //The amount of hops done so far
} ShipState;

//Helper function to find a routepoint in or add it to the route
RoutePoint* find_or_add_routepoint(unsigned int planet_id, Route *route)
{
    RoutePoint *query = route->first;
	while (query != NULL)
    {
		if(query->planet_id == planet_id)
        {
			return query;
		}
		query = query->next;
	}

    RoutePoint *routePoint = malloc(sizeof(RoutePoint));
    routePoint->planet_id = planet_id;
    routePoint->visited = false;
    routePoint->connections = NULL;
    routePoint->num_connections = 0;
    routePoint->distance_from_mixer = 0;
    routePoint->next = NULL;

	route->size++;
	if (route->first == NULL)
    {
		route->first = routePoint;
		route->last = routePoint;
		return routePoint;
	}
	route->last->next = routePoint;
	route->last = routePoint;
    return routePoint;
}


//Helper function to check if a routepoint is exhausted
bool is_exhausted(RoutePoint *routePoint)
{
	for (int i = 0; i < routePoint->num_connections; ++i)
    {
		if(!routePoint->connections[i]->visited)
        {
			return false;
		}
	}
	return true;
}

//Helper function to get the routepoint closest to the mixer that's not exhausted
RoutePoint* find_closest_routepoint(Route *route)
{
    RoutePoint *closest = NULL;
    double closest_distance = DBL_MAX;

    RoutePoint *current = route->first;
    while (current != NULL)
    {
        if (current->visited && current->distance_from_mixer < closest_distance)
        {
            if (!is_exhausted(current))
            {
                closest_distance = current->distance_from_mixer;
                closest = current;
            }
        }
        current = current->next;
    }
    return closest;
}

ShipAction space_hop(unsigned int crt_planet,
                     unsigned int *connections,
                     int num_connections,
                     double distance_from_mixer,
                     void *ship_state)
{
	ShipAction action;
	ShipState *state = (ShipState*)ship_state;

	if (state == NULL) //Init ShipState if it does not exist
    {
		state = malloc(sizeof(ShipState));

		state->route = malloc(sizeof(Route));
		state->route->first = NULL;
		state->route->last = NULL;
		state->route->size = 0;

		state->current = NULL;
		state->next = NULL;
		state->num_hops = 0;
	}

    state->num_hops++; //Increment num_hops

    if (state->next == NULL)
    {
        state->current = find_or_add_routepoint(crt_planet, state->route); //If hop was random, create current
    }
    else
    {
        state->current = state->next; //Otherwise take it from the state
    }

    if (!state->current->visited) //If this planet wasn't visited, update it's information in the route
    {
        state->current->distance_from_mixer = distance_from_mixer;
        state->current->visited = true;
        state->current->num_connections = num_connections;
        state->current->connections = malloc(num_connections * sizeof(RoutePoint*));
        
        for (int i = 0; i < num_connections; i++)
        {
            state->current->connections[i] = find_or_add_routepoint(connections[i], state->route);
        }
    }

    if (state->num_hops == 2) //Viset planet 0 first
    {
        state->next = find_or_add_routepoint(0, state->route);
        action.next_planet = 0;
    }
    else if (state->num_hops < 9) //Do random hops for the first X hops
    {
        state->next = NULL;
        action.next_planet = RAND_PLANET;
    }
    else
    {
        RoutePoint *closest = find_closest_routepoint(state->route);
        if (closest == NULL) //If there's no non-exhausted planet closer to the mixer, random jump
        {
            state->next = NULL;
            action.next_planet = RAND_PLANET;
        }
        else //Otherwise go to a neighbour of the closest visited planet
        {
            for (int i = 0; i < closest->num_connections; i++)
            {
                if (!closest->connections[i]->visited)
                {
                    state->next = closest->connections[i];
                    action.next_planet = state->next->planet_id;
                }
            }
        }
    }

	action.ship_state = state;
	return action;
}
#ifndef PYVRP_PROBLEMDATA_H
#define PYVRP_PROBLEMDATA_H

#include "Matrix.h"
#include "Measure.h"

#include <iosfwd>
#include <optional>
#include <vector>

namespace pyvrp
{
/**
 * ProblemData(
 *     clients: List[Client],
 *     vehicle_types: List[VehicleType],
 *     distance_matrix: List[List[int]],
 *     duration_matrix: List[List[int]],
 * )
 *
 * Creates a problem data instance. This instance contains all information
 * needed to solve the vehicle routing problem.
 *
 * Parameters
 * ----------
 * clients
 *     List of clients. The first client (at index 0) is assumed to be the
 *     depot. The time window for the depot is assumed to describe the overall
 *     time horizon. The depot should have 0 demand and 0 service duration.
 * vehicle_types
 *     List of vehicle types in the problem instance.
 * distance_matrix
 *     A matrix that gives the distances between clients (and the depot at
 *     index 0).
 * duration_matrix
 *     A matrix that gives the travel times between clients (and the depot at
 *     index 0).
 */
class ProblemData
{
public:
    /**
     * Client(
     *    x: int,
     *    y: int,
     *    demand: int = 0,
     *    service_duration: int = 0,
     *    tw_early: int = 0,
     *    tw_late: int = 0,
     *    release_time: int = 0,
     *    prize: int = 0,
     *    required: bool = True,
     * )
     *
     * Simple data object storing all client data as (read-only) properties.
     *
     * Parameters
     * ----------
     * x
     *     Horizontal coordinate of this client, that is, the 'x' part of the
     *     client's (x, y) location tuple.
     * y
     *     Vertical coordinate of this client, that is, the 'y' part of the
     *     client's (x, y) location tuple.
     * demand
     *     The amount this client's demanding. Default 0.
     * service_duration
     *     This client's service duration, that is, the amount of time we need
     *     to visit the client for. Service should start (but not necessarily
     *     end) within the [:py:attr:`~tw_early`, :py:attr:`~tw_late`] interval.
     *     Default 0.
     * tw_early
     *     Earliest time at which we can visit this client. Default 0.
     * tw_late
     *     Latest time at which we can visit this client. Default 0.
     * release_time
     *     Earliest time at which this client is released, that is, the earliest
     *     time at which a vehicle may leave the depot to visit this client.
     *     Default 0.
     * dispatch_time
     *     Latest time at which this client should be dispatched, that is, the
     *     latest time at which a vehicle may leave the depot to visit this
     *     client. Default 0.
     * prize
     *     Prize collected by visiting this client. Default 0.
     * required
     *     Whether this client must be part of a feasible solution. Default
     *     True.
     */
    struct Client
    {
        Coordinate const x;
        Coordinate const y;
        Load const demand;
        Duration const serviceDuration;
        Duration const twEarly;       // Earliest possible start of service
        Duration const twLate;        // Latest possible start of service
        Duration const releaseTime;   // Earliest possible time to leave depot
        Duration const dispatchTime;  // Latest possible time to leave depot
        Cost const prize = 0;         // Prize for visiting this client
        bool const required = true;   // Must client be in solution?

        Client(Coordinate x,
               Coordinate y,
               Load demand = 0,
               Duration serviceDuration = 0,
               Duration twEarly = 0,
               Duration twLate = 0,
               Duration releaseTime = 0,
               Duration dispatchTime = 0,
               Cost prize = 0,
               bool required = true);
    };

    /**
     * VehicleType(
     *     capacity: int,
     *     num_available: int,
     *     fixed_cost: int = 0,
     *     tw_early: Optional[int] = None,
     *     tw_late: Optional[int] = None,
     * )
     *
     * Simple data object storing all vehicle type data as properties.
     *
     * .. note::
     *
     *    If ``tw_early`` is set, then also ``tw_late`` must be provided to
     *    completely specify the shift duration (and vice versa). If neither
     *    are given, the shift duration defaults to the depot's time window.
     *
     * Parameters
     * ----------
     * capacity
     *     Capacity (maximum total demand) of this vehicle type. Must be
     *     non-negative.
     * num_available
     *     Number of vehicles of this type that are available. Must be positive.
     * fixed_cost
     *     Fixed cost of using a vehicle of this type. Default 0.
     * tw_early
     *     Start of the vehicle type's shift. Defaults to the depot's opening
     *     time if not given.
     * tw_late
     *     End of the vehicle type's shift. Defaults to the depot's closing
     *     time if not given.
     *
     * Attributes
     * ----------
     * capacity
     *     Capacity (maximum total demand) of this vehicle type.
     * num_available
     *     Number of vehicles of this type that are available.
     * depot
     *     Depot associated with these vehicles.
     * fixed_cost
     *     Fixed cost of using a vehicle of this type.
     * tw_early
     *     Start of the vehicle type's shift, if specified.
     * tw_late
     *     End of the vehicle type's shift, if specified.
     */
    struct VehicleType
    {
        Load const capacity;        // This type's vehicle capacity
        size_t const numAvailable;  // Available vehicles of this type
        size_t const depot = 0;     // Departure and return depot location
        Cost const fixedCost;       // Fixed cost of using this vehicle type
        std::optional<Duration> const twEarly;  // Start of shift
        std::optional<Duration> const twLate;   // End of shift

        VehicleType(Load capacity,
                    size_t numAvailable,
                    Cost fixedCost = 0,
                    std::optional<Duration> twEarly = std::nullopt,
                    std::optional<Duration> twLate = std::nullopt);
    };

private:
    std::pair<double, double> centroid_;           // Center of client locations
    Matrix<Distance> const dist_;                  // Distance matrix
    Matrix<Duration> const dur_;                   // Duration matrix
    std::vector<Client> const clients_;            // Client/depot information
    std::vector<VehicleType> const vehicleTypes_;  // Vehicle type information

    size_t const numClients_;
    size_t const numVehicleTypes_;
    size_t const numVehicles_;

public:
    /**
     * Returns client data for the given client.
     *
     * Parameters
     * ----------
     * client
     *     Client number whose information to retrieve.
     *
     * Returns
     * -------
     * Client
     *     A simple data object containing the requested client's information.
     */
    [[nodiscard]] inline Client const &client(size_t client) const;

    /**
     * Center point of all client locations (excluding the depot).
     *
     * Returns
     * -------
     * tuple
     *     Centroid of all client locations.
     */
    [[nodiscard]] std::pair<double, double> const &centroid() const;

    /**
     * Returns vehicle type data for the given vehicle type.
     *
     * Parameters
     * ----------
     * vehicle_type
     *     Vehicle type number whose information to retrieve.
     *
     * Returns
     * -------
     * VehicleType
     *     A simple data object containing the vehicle type information.
     */
    [[nodiscard]] inline VehicleType const &
    vehicleType(size_t vehicleType) const;

    /**
     * Returns the travel distance between the first and second argument,
     * according to this instance's travel distance matrix.
     *
     * Parameters
     * ----------
     * first
     *     Client or depot number.
     * second
     *     Client or depot number.
     *
     * Returns
     * -------
     * int
     *     Travel distance between the given clients.
     */
    [[nodiscard]] inline Distance dist(size_t first, size_t second) const;

    /**
     * Returns the travel duration between the first and second argument,
     * according to this instance's travel duration matrix.
     *
     * Parameters
     * ----------
     * first
     *     Client or depot number.
     * second
     *     Client or depot number.
     *
     * Returns
     * -------
     * int
     *     Travel duration between the given clients.
     */
    [[nodiscard]] inline Duration duration(size_t first, size_t second) const;

    /**
     * @return The full travel distance matrix.
     */
    [[nodiscard]] inline Matrix<Distance> const &distanceMatrix() const;

    /**
     * @return The full travel duration matrix.
     */
    [[nodiscard]] inline Matrix<Duration> const &durationMatrix() const;

    /**
     * Number of clients in this problem instance.
     *
     * Returns
     * -------
     * int
     *     Number of clients in the instance.
     */
    [[nodiscard]] size_t numClients() const;

    /**
     * Number of vehicle types in this problem instance.
     *
     * Returns
     * -------
     * int
     *     Number of vehicle types in this problem instance.
     */
    [[nodiscard]] size_t numVehicleTypes() const;

    /**
     * Number of vehicles in this problem instance.
     *
     * Returns
     * -------
     * int
     *     Number of vehicles in this problem instance.
     */
    [[nodiscard]] size_t numVehicles() const;

    /**
     * Returns a new ProblemData instance with the same data as this instance,
     * except for the given parameters, which are used instead.
     *
     * Parameters
     * ----------
     * clients
     *    Optional list of clients (including depot at index 0).
     * vehicle_types
     *    Optional list of vehicle types.
     * distance_matrix
     *    Optional distance matrix.
     * duration_matrix
     *    Optional duration matrix.
     *
     * Returns
     * -------
     * ProblemData
     *    A new ProblemData instance with possibly replaced data.
     * */
    ProblemData replace(std::optional<std::vector<Client>> &clients,
                        std::optional<std::vector<VehicleType>> &vehicleTypes,
                        std::optional<Matrix<Distance>> &distMat,
                        std::optional<Matrix<Duration>> &durMat);

    /**
     * Constructs a ProblemData object with the given data. Assumes the list
     * of clients contains the depot, such that each vector is one longer
     * than the number of clients.
     *
     * @param clients      List of clients (including depot at index 0).
     * @param vehicleTypes List of vehicle types.
     * @param distMat      Distance matrix.
     * @param durMat       Duration matrix.
     */
    ProblemData(std::vector<Client> const &clients,
                std::vector<VehicleType> const &vehicleTypes,
                Matrix<Distance> distMat,
                Matrix<Duration> durMat);
};

ProblemData::Client const &ProblemData::client(size_t client) const
{
    return clients_[client];
}

ProblemData::VehicleType const &
ProblemData::vehicleType(size_t vehicleType) const
{
    return vehicleTypes_[vehicleType];
}

Distance ProblemData::dist(size_t first, size_t second) const
{
    return dist_(first, second);
}

Duration ProblemData::duration(size_t first, size_t second) const
{
    return dur_(first, second);
}

Matrix<Distance> const &ProblemData::distanceMatrix() const { return dist_; }

Matrix<Duration> const &ProblemData::durationMatrix() const { return dur_; }
}  // namespace pyvrp

#endif  // PYVRP_PROBLEMDATA_H

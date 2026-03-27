#ifndef DATAPOOL_INCLUDE_TASK_TYPEDEF_H_
#define DATAPOOL_INCLUDE_TASK_TYPEDEF_H_

#include "xkit/planning/datapool/public_typedef.h"
#include "xkit/planning/datapool/task_input.h"
#include "xkit/planning/datapool/perception_input.h"
#include "xkit/planning/datapool/localization_input.h"
#include "xkit/planning/datapool/debug_message.h"
#include "xkit/planning/datapool/common_config.h"
#include "xkit/planning/datapool/planning_info.h"
#include "PlanningStatus.h"
#include "task_struct.h"
// #include "src/execution/business/include/business_base.h"
// #include "src/execution/cognition/cognition_base/include/cognition_base.h"
// #include "src/execution/behaviorplan/behavior_base/behavior_context_base.h"
// #include "src/execution/speedplan/speedplan_base/include/speedplan_base.h"
// #include "algorithm_base.h"

namespace avos {
    namespace planning {


        enum class eTaskExeResult {
            DEFAULT = 0,
            SUCCESS,
            FAILURE,
            FUNCTION_ERROR
        };

/**
* @brief define the task status enum for task fsm
*/
        enum class eTaskStatus {
            DEFAULT = 0,    /*!< default is zero */
            IDLE_TASK,
            EXECUTE_TASK,
            EXECEPTIONAL_TASK,
            END_TASK
        };
        typedef struct TaskStatus {
            eTaskStatus status;
            std::string str_status;

            TaskStatus() {
                Reset();
            }

            void Reset() {
                status = eTaskStatus::DEFAULT;
                str_status = "DEFAULT";
            }

            void SetIdleTask() {
                status = eTaskStatus::IDLE_TASK;
                str_status = "IDLE_TASK";
            }

            void SetExecuteTask() {
                status = eTaskStatus::EXECUTE_TASK;
                str_status = "EXECUTE_TASK";
            }

            void SetEndTask() {
                status = eTaskStatus::END_TASK;
                str_status = "END_TASK";
            }

            void SetExceptionalTask() {
                status = eTaskStatus::EXECEPTIONAL_TASK;
                str_status = "EXECEPTIONAL_TASK";
            }
        } TaskStatus;

        enum class eTaskAnalyse {
            DEFAULT = 0,
            EXECUTABLE,
            UNEXECUTABLE,
            END_TASK
        };
        typedef struct TaskAnalyseResult {
            eTaskAnalyse status;
            std::string str_status;

            TaskAnalyseResult() {
                status = eTaskAnalyse::DEFAULT;
                str_status = " ";
            }
        } TaskAnalyseResult;

        enum class eSubTaskStatus {
            DEFAULT = 0,    /*!< default is zero */
            OK,       /*!< sub task is exe ok */
            EXCPETION,  /*!< sub task has exception */
            REASON1,  /*!< sub task has REASON1 */
            REASON2,    /*!< sub task has REASON2 */
            REASON3,     /*!< sub task has REASON3 */
        };
        typedef struct ExeSubTaskResult {
            eSubTaskStatus status;

            ExeSubTaskResult() {
                status = eSubTaskStatus::DEFAULT;
            }
        } ExeSubTaskResult;

        enum class eEndTaskHandleRes {
            DEFAULT = 0,
            HANDING_OVER,
            HANDLING
        };
        typedef struct EndTaskHandleResult {
            eEndTaskHandleRes status;

            EndTaskHandleResult() {
                status = eEndTaskHandleRes::DEFAULT;
            }
        } EndTaskHandleResult;


        typedef struct TaskFSMInfo {
            TaskStatus task_status;
            TaskAnalyseResult task_analyse_result_;
            ExeSubTaskResult sub_task_result_;
            EndTaskHandleResult endtask_handle_result_;

            TaskFSMInfo() {
                task_status.Reset();
                Reset();
            }

            // clear flags
            void ResetAnalyseStatus() {
                task_analyse_result_.status = eTaskAnalyse::DEFAULT;
            }

            void ResetExeSubTaskStatus() {
                sub_task_result_.status = eSubTaskStatus::DEFAULT;
            }

            void ResetEndTaskHandleStatus() {
                endtask_handle_result_.status = eEndTaskHandleRes::DEFAULT;
            }

            // set method
            void setTaskOver() {
                task_analyse_result_.status = eTaskAnalyse::END_TASK;
            }

            void setTaskExecutable() {
                task_analyse_result_.status = eTaskAnalyse::EXECUTABLE;
            }

            void setTaskUnexecutable() {
                task_analyse_result_.status = eTaskAnalyse::UNEXECUTABLE;
            }

            void setTaskExeOK() {
                sub_task_result_.status = eSubTaskStatus::OK;
            }

            void setTaskExeFault() {
                sub_task_result_.status = eSubTaskStatus::EXCPETION;
            }

            void setEndTaskHandleOver() {
                endtask_handle_result_.status = eEndTaskHandleRes::HANDING_OVER;
            }

            // get method
            bool IsTaskOver() {
                return (task_analyse_result_.status == eTaskAnalyse::END_TASK);
            }

            bool IsTaskExecutable() {
                return (task_analyse_result_.status == eTaskAnalyse::EXECUTABLE);
            }

            bool IsTaskUnexecutable() {
                return (task_analyse_result_.status == eTaskAnalyse::UNEXECUTABLE);
            }

            bool IsTaskExeOK() {
                return (sub_task_result_.status == eSubTaskStatus::OK);
            }

            bool IsTaskExeFault() {
                return (sub_task_result_.status == eSubTaskStatus::EXCPETION);
            }

            bool IsEndTaskHandleOver() {
                return (endtask_handle_result_.status == eEndTaskHandleRes::HANDING_OVER);
            }

            void Reset() {
                ResetAnalyseStatus();
                ResetExeSubTaskStatus();
                ResetEndTaskHandleStatus();
            }
        } TaskFSMInfo;

// class AlgorithmBase;
        class BehaviorContextBase;

        typedef struct FollowProperty_ {
            char follow_type;  //default_description
            float safe_distance;  //default_description
            float max_speed;  //default_description
            char follow_mode;  //default_description
            bool avoid;  //default_description
        } FollowProperty_;
        typedef struct FollowTarget {
            long long id; //default_description
            char target_origin;  //default_description
            double lon;  //default_description
            double lat;  //default_description
            double x;  //default_description
            double y;  //default_description
            double heading;  //default_description
            double speed;  //default_description
            double model_lon;  //default_description
            double model_lat;  //default_description
            double model_length; //default_description
            double model_width;  //default_description
        } FollowTarget;
        typedef struct Follow {
            bool invalid;  //default_description
            FollowProperty_ property; //default_description
            FollowTarget target; //default_description
        } Follow;
        typedef struct TaskFSMParam {
            TaskContent task_content;
            TaskFSMInfo task_fsm_info;
            LocalizationData localization_data;
            eTaskExeResult task_exe_status;
            PlanningInfo planning_info;
            ConfigInfo config_info;
            Paths paths;
            FaultHandleInfo fault_handle;
            VehicleStatus vehicle_status;
            InnerPath track_inner_paths_data;
            PlanningStatus mission_status;
            int follow_index_diff;
            Follow follow_app;
            Follow follow_per;

            // std::shared_ptr<AlgorithmBase> algorithm_ptr;
            // std::shared_ptr<AlgorithmBase> last_algorithm_ptr;
            // std::shared_ptr<avos::planning::CognitionBase> cognition_ptr;
            // std::shared_ptr<BehaviorContextBase> behavior_ptr;
            // std::shared_ptr<avos::motionplan::SpeedplanBase> speed_plan_ptr;
            // std::shared_ptr<BehaviorContextBase> last_behavior_context_ptr;
            std::string algorithm_str;
            std::string last_algorithm_str;
            std::string cognition_str;
            std::string behavior_str;
            std::string speed_plan_str;
            std::string last_behavior_context_str;
            DebugPlanningMsg debug_planning_msg;

            TaskFSMParam() {
                Reset();
            }

            void Reset() {
                task_content.Reset();
                task_fsm_info.Reset();
                fault_handle.Reset();
                task_exe_status = eTaskExeResult::DEFAULT;
                debug_planning_msg.Reset();
                algorithm_str = "null";
                last_algorithm_str = "null";
                cognition_str = "null";
                behavior_str = "null";
                speed_plan_str = "null";
                last_behavior_context_str = "null";
                // algorithm_ptr.reset();
                // last_algorithm_ptr.reset();
                // behavior_ptr.reset();
                // last_behavior_context_ptr.reset();
                vehicle_status.Reset();
                follow_index_diff = 0;
//    mission_status.mission_type = -1;
                mission_status.command_status = 0;
            }
        } TaskFSMParam;


    } // namespace planning
} // namespace avos

#endif // DATAPOOL_INCLUDE_TASK_INPUT_H_

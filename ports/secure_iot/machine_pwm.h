#include <stdbool.h>
#include <stdint.h>
/**
 * @file machine_pwm.h
 * @author Biancaa.R (biancaa2210329@ssn.edu.in)
 * @brief Contains function prototypes of PWM drivers for micropython
 * @version 1.0
 * @date 2025-7-15
 * 
 * @copyright Copyright (c) Mindgrove Technologies Pvt. Ltd 2024. All rights reserved.
 * 
 */
#ifndef MACHINE_PWM_H
#define MACHINE_PWM_H
// Forward declarations
/**
 * @fn mp_machine_pwm_print
 * 
 * @brief This function prints the information about the given PWM instance.
 * 
 * @param takes the print type pointer to use for printing,type and the main pwm object used.
 * @return void
 */
//extern machine_pwm_obj_t;
typedef struct _machine_pwm_obj_t machine_pwm_obj_t;
static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
/**
 * @fn mp_machine_pwm_init_helper
 * 
 * @brief This function helps by taking keyword arguments during init and mapping them to the correct variables.
 * 
 * @param takes the main pwm object used and passed positional,kw arguments.
 * @return void
 */
static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
/**
 * @fn mp_machine_pwm_make_new
 * 
 * @brief This function is called for making a new pwm instance
 * 
 * @param takes passed positional,kw arguments.
 * @return pwm_object type
 */
static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
/**
 * @fn mp_machine_pwm_deinit
 * 
 * @brief This function helps to clear a pwm instance
 * 
 * @param takes the main pwm object used 
 * @return void
 */
static void mp_machine_pwm_deinit(machine_pwm_obj_t *self);
/**
 * @fn mp_machine_pwm_freq_get
 * 
 * @brief This function helps to get the value for frequency
 * 
 * @param takes the main pwm object used 
 * @return void
 */
static void mp_machine_pwm_freq_get(machine_pwm_obj_t *self);
/**
 * @fn mp_machine_pwm_freq_set
 * 
 * @brief This function helps to set the value for frequency
 * 
 * @param takes the main pwm object used 
 * @return void
 */
static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq);
/**
 * @fn set_active
 * 
 * @brief This function helps to set the pwm instance active ,if made inactive
 * 
 * @param takes the main pwm object used 
 * @return void
 */
static void set_active(machine_pwm_obj_t *self);
/**
 * @fn mp_machine_pwm_duty_get
 * 
 * @brief This function helps to get the value for duty cycle
 * 
 * @param takes the main pwm object used 
 * @return the duty value as an mp_obj_t
 */
static mp_obj_t mp_machine_pwm_duty_get(machine_pwm_obj_t *self);
/**
 * @fn mp_machine_pwm_duty_set
 * 
 * @brief This function helps to set the value for duty
 * 
 * @param takes the main pwm object used 
 * @return void
 */
static void mp_machine_pwm_duty_set(machine_pwm_obj_t *self, mp_int_t duty);


#endif
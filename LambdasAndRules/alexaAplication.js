const Alexa = require('ask-sdk-core');
const AWS = require('aws-sdk');
const IotData = new AWS.IotData({endpoint: 'a255q5ixlivm2d-ats.iot.us-east-2.amazonaws.com'});

let ShadowParams = {
    thingName: 'coso_esp_temperatura',
};

function getShadowPromise(params) {
    return new Promise((resolve, reject) => {
        IotData.getThingShadow(params, (err, data) => {
            if (err) {
                console.log(err, err.stack);
                reject('Failed to get thing shadow ${err.errorMessage}');
            } else {
                console.log("datos obtenidos: ", data.payload);
                resolve(JSON.parse(data.payload));
            }
        });
    });
}

const LaunchRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'LaunchRequest';
    },
    handle(handlerInput) {
        const speakOutput = 'Bienvenido a tu objeto inteligente, puedes consultar la temperatura ambiente o del la superficie a la que apuntes el sensor ¿Qué deseas hacer?';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const AmbientIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'ambientIntent';
    },
    async handle(handlerInput) {
        try {
            console.log("entrando a ambient intent");
            const result = await getShadowPromise(ShadowParams);
            const tempDHT = result.state.reported.tempDHT;
            console.log("temps: ", tempDHT);
            const speakOutput = `La temperatura ambiente es de ${tempDHT.toFixed(1)} grados Celsius.`;

            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();
        } catch (error) {
            return handlerInput.responseBuilder
                .speak('Hubo un error al obtener la temperatura ambiente. Por favor, intenta nuevamente.')
                .reprompt(speakOutput)
                .getResponse();
        }
    },
};

const ObjectIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'objectIntent';
    },
    async handle(handlerInput) {
        try {
            console.log("entrando a object intent");
            const result = await getShadowPromise(ShadowParams);
            const tempMLX = result.state.reported.tempMLX;
            console.log("temps: ", tempMLX);
            const speakOutput = `La temperatura del objeto es de ${tempMLX.toFixed(1)} grados Celsius.`;

            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();
        } catch (error) {
            return handlerInput.responseBuilder
                .speak('Hubo un error al obtener la temperatura del objeto. Por favor, intenta nuevamente.')
                .reprompt(speakOutput)
                .getResponse();
        }
    },
};

const StateIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'stateIntent';
    },
    async handle(handlerInput) {
        try {
            console.log("entrando a state intent");
            const result = await getShadowPromise(ShadowParams);
            const tempDHT = result.state.reported.tempDHT;
            const tempMLX = result.state.reported.tempMLX;
            console.log("temps: ", tempDHT, tempMLX);
            const speakOutput = `La temperatura ambiente es de ${tempDHT.toFixed(1)} grados Celsius, y la temperatura del objeto es de ${tempMLX.toFixed(1)} grados Celsius.`;

            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();
        } catch (error) {
            return handlerInput.responseBuilder
                .speak('Hubo un error al obtener las temperaturas. Por favor, intenta nuevamente.')
                .reprompt(speakOutput)
                .getResponse();
        }
    },
};

const changeSendIntervalHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'changeSendInterval';
    },
    async handle(handlerInput) {
        try {
            console.log("entrando a changeSendInterval");

            // Recuperar el valor del slot sendInterval
            const intervalSlot = handlerInput.requestEnvelope.request.intent.slots.sendInterval;

            if (!intervalSlot || !intervalSlot.value) {
                // Si el slot está vacío, responde con un mensaje de error
                const speakOutput = 'Por favor, proporciona un intervalo de tiempo válido (por ejemplo, 5 minutos o 30 segundos).';
                return handlerInput.responseBuilder
                    .speak(speakOutput)
                    .getResponse();
            }

            const intervalText = intervalSlot.value.toLowerCase();
            console.log(`Texto del intervalo: ${intervalText}`);

            // Usar una expresión regular para separar el número y la unidad
            const match = intervalText.match(/(\d+)\s*(segundos|minutos|minuto|segundo)/);

            if (!match) {
                const speakOutput = 'Por favor, proporciona un intervalo válido con una unidad (segundos o minutos).';
                return handlerInput.responseBuilder
                    .speak(speakOutput)
                    .getResponse();
            }

            const interval = parseInt(match[1], 10);
            const unit = match[2];

            // Convertir a milisegundos según la unidad
            let intervalInMs;
            if (unit == 'segundos' || unit == 'segundo') {
                intervalInMs = interval * 1000;  // Convertir segundos a milisegundos
            } else if (unit == 'minutos' || unit == 'minuto') {
                intervalInMs = interval * 60000;  // Convertir minutos a milisegundos
            }

            // Ahora tienes el valor del intervalo en milisegundos
            console.log(`Intervalo en milisegundos: ${intervalInMs}`);

            // Actualizar el shadow con el nuevo intervalo en desired
            const UpdateParams = {
                thingName: 'coso_esp_temperatura',  // Asegúrate de usar el nombre correcto del thing
                payload: JSON.stringify({
                    state: {
                        desired: {
                            sendInterval: intervalInMs  // Actualiza el valor en desired
                        }
                    }
                })
            };

            const updateShadow = new Promise((resolve, reject) => {
                IotData.updateThingShadow(UpdateParams, (err, data) => {
                    if (err) {
                        console.log("Error al actualizar el shadow:", err, err.stack);
                        reject(`Failed to update thing shadow: ${err.errorMessage}`);
                    } else {
                        console.log("Shadow actualizado exitosamente:", data);
                        resolve(data);  // Respuesta con los datos de la actualización
                    }
                });
            });

            // Esperamos a que la promesa se resuelva
            await updateShadow;

            // Responder al usuario
            const speakOutput = `El intervalo de envío de datos se cambió a ${interval} ${unit}.`;

            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();

        } catch (error) {
            console.error('Error al actualizar el intervalo: ', error);
            const speakOutput = 'Hubo un error al setear el nuevo intervalo. Por favor, intenta nuevamente.';
            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();
        }
    },
};


const changeTemperatureTreshHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'changeTemperatureTresh';
    },
    async handle(handlerInput) {
        try {
            console.log("Entrando a changeTemperatureTresh");

            // Recuperar el valor del slot tempTresh
            const tempTreshSlot = handlerInput.requestEnvelope.request.intent.slots.tempTresh;

            if (!tempTreshSlot || !tempTreshSlot.value) {
                // Si el slot está vacío o no se reconoce correctamente, responde con un mensaje de error
                const speakOutput = 'Por favor, proporciona un valor válido para el umbral de temperatura.';
                return handlerInput.responseBuilder
                    .speak(speakOutput)
                    .getResponse();
            }

            const tempTresh = parseInt(tempTreshSlot.value, 10); // Convertir el valor a número

            // Verificar que el valor esté dentro de un rango válido (por ejemplo, 0 a 100)
            if (isNaN(tempTresh) || tempTresh < 0 || tempTresh > 100) {
                const speakOutput = 'El umbral de temperatura debe ser un valor entre 0 y 100 grados.';
                return handlerInput.responseBuilder
                    .speak(speakOutput)
                    .getResponse();
            }

            // Actualizar el shadow con el nuevo valor de tempTresh
            const UpdateParams = {
                thingName: 'coso_esp_temperatura',  // Asegúrate de usar el nombre correcto del thing
                payload: JSON.stringify({
                    state: {
                        desired: {
                            tempTresh: tempTresh  // Actualiza el valor en desired
                        }
                    }
                })
            };

            const updateShadow = new Promise((resolve, reject) => {
                IotData.updateThingShadow(UpdateParams, (err, data) => {
                    if (err) {
                        console.log("Error al actualizar el shadow:", err, err.stack);
                        reject(`Failed to update thing shadow: ${err.errorMessage}`);
                    } else {
                        console.log("Shadow actualizado exitosamente:", data);
                        resolve(data);  // Respuesta con los datos de la actualización
                    }
                });
            });

            // Esperamos a que la promesa se resuelva
            await updateShadow;

            // Responder al usuario
            const speakOutput = `El umbral de temperatura significativa se ha cambiado a ${tempTresh} grados.`;

            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();

        } catch (error) {
            console.error('Error al actualizar el umbral de temperatura: ', error);
            const speakOutput = 'Hubo un error al cambiar el umbral de temperatura. Por favor, intenta nuevamente.';
            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();
        }
    },
};

const changeAlarmTempHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'changeAlarmTemp';
    },
    async handle(handlerInput) {
        try {
            console.log("Entrando a changeAlarmTemp");

            // Recuperar el valor del slot alarmTemp
            const alarmTempSlot = handlerInput.requestEnvelope.request.intent.slots.alarmTemp;

            if (!alarmTempSlot || !alarmTempSlot.value) {
                // Si el slot está vacío o no se reconoce correctamente, responde con un mensaje de error
                const speakOutput = 'Por favor, proporciona un valor válido para la temperatura de alarma.';
                return handlerInput.responseBuilder
                    .speak(speakOutput)
                    .getResponse();
            }

            const alarmTemp = parseInt(alarmTempSlot.value, 10); // Convertir el valor a número

            // Verificar que el valor esté dentro de un rango válido (por ejemplo, 0 a 100)
            if (isNaN(alarmTemp) || alarmTemp < 0 || alarmTemp > 100) {
                const speakOutput = 'La temperatura de alarma debe ser un valor entre 0 y 100 grados.';
                return handlerInput.responseBuilder
                    .speak(speakOutput)
                    .getResponse();
            }

            // Actualizar el shadow con el nuevo valor de alarmTemp
            const UpdateParams = {
                thingName: 'coso_esp_temperatura',  // Asegúrate de usar el nombre correcto del thing
                payload: JSON.stringify({
                    state: {
                        desired: {
                            alarmTemp: alarmTemp  // Actualiza el valor en desired
                        }
                    }
                })
            };

            const updateShadow = new Promise((resolve, reject) => {
                IotData.updateThingShadow(UpdateParams, (err, data) => {
                    if (err) {
                        console.log("Error al actualizar el shadow:", err, err.stack);
                        reject(`Failed to update thing shadow: ${err.errorMessage}`);
                    } else {
                        console.log("Shadow actualizado exitosamente:", data);
                        resolve(data);  // Respuesta con los datos de la actualización
                    }
                });
            });

            // Esperamos a que la promesa se resuelva
            await updateShadow;

            // Responder al usuario
            const speakOutput = `La temperatura de alarma se ha cambiado a ${alarmTemp} grados.`;

            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();

        } catch (error) {
            console.error('Error al actualizar la temperatura de alarma: ', error);
            const speakOutput = 'Hubo un error al cambiar la temperatura de alarma. Por favor, intenta nuevamente.';
            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();
        }
    },
};


const HelpIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.HelpIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Puedes consultar la temperatura ambiente o del la superficio a al que apuntes el sensor.';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const CancelAndStopIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && (Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.CancelIntent'
                || Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.StopIntent');
    },
    handle(handlerInput) {
        const speakOutput = 'Hasta pronto!';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .getResponse();
    }
};
/* *
 * FallbackIntent triggers when a customer says something that doesn’t map to any intents in your skill
 * It must also be defined in the language model (if the locale supports it)
 * This handler can be safely added but will be ingnored in locales that do not support it yet 
 * */
const FallbackIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.FallbackIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Lo siento, no entendí, intenta de nuevo.';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};
/* *
 * SessionEndedRequest notifies that a session was ended. This handler will be triggered when a currently open 
 * session is closed for one of the following reasons: 1) The user says "exit" or "quit". 2) The user does not 
 * respond or says something that does not match an intent defined in your voice model. 3) An error occurs 
 * */
const SessionEndedRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'SessionEndedRequest';
    },
    handle(handlerInput) {
        console.log(`~~~~ Session ended: ${JSON.stringify(handlerInput.requestEnvelope)}`);
        // Any cleanup logic goes here.
        return handlerInput.responseBuilder.getResponse(); // notice we send an empty response
    }
};
/* *
 * The intent reflector is used for interaction model testing and debugging.
 * It will simply repeat the intent the user said. You can create custom handlers for your intents 
 * by defining them above, then also adding them to the request handler chain below 
 * */
const IntentReflectorHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest';
    },
    handle(handlerInput) {
        const intentName = Alexa.getIntentName(handlerInput.requestEnvelope);
        const speakOutput = `Intentó ejecutar ${intentName}`;

        return handlerInput.responseBuilder
            .speak(speakOutput)
            //.reprompt('add a reprompt if you want to keep the session open for the user to respond')
            .getResponse();
    }
};
/**
 * Generic error handling to capture any syntax or routing errors. If you receive an error
 * stating the request handler chain is not found, you have not implemented a handler for
 * the intent being invoked or included it in the skill builder below 
 * */
const ErrorHandler = {
    canHandle() {
        return true;
    },
    handle(handlerInput, error) {
        const speakOutput = 'Disculpa, hubo un error. Intenta de nuevo.';
        console.log(`~~~~ Error handled: ${JSON.stringify(error)}`);

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

/**
 * This handler acts as the entry point for your skill, routing all request and response
 * payloads to the handlers above. Make sure any new handlers or interceptors you've
 * defined are included below. The order matters - they're processed top to bottom 
 * */
exports.handler = Alexa.SkillBuilders.custom()
    .addRequestHandlers(
        LaunchRequestHandler,
        AmbientIntentHandler,
        ObjectIntentHandler,
        StateIntentHandler,
        changeSendIntervalHandler,
        changeTemperatureTreshHandler,
        changeAlarmTempHandler,
        HelpIntentHandler,
        CancelAndStopIntentHandler,
        FallbackIntentHandler,
        SessionEndedRequestHandler,
        IntentReflectorHandler)
    .addErrorHandlers(
        ErrorHandler)
    .withCustomUserAgent('sample/hello-world/v1.2')
    .lambda();
    
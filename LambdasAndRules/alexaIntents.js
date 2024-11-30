//fragmentos de codigo extraidos de alexaAplication.js

//stateIntent:
//se desencadena con los siguientes utterances:
//- dame las dos temperaturas
//- dime ambas temperaturas
//- quiero saber ambas temperaturas
//- dame un reporte completo

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


// ambientIntent:
// se desencadena con los siguientes utterances:
// - a que temperatura estamos
// - cual es la temperatura ambientIntent


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


// objectIntent:
// se desencadena con los siguientes utterances:
// - a que temperatura esta esto
// - cual es la temperatura de esta superficie
// - cual es mi temperatura

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
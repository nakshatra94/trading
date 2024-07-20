// App.js
import React, { useState, useEffect } from 'eact';
import { View, Text, StyleSheet, Button } from 'eact-native';
import axios from 'axios';

const App = () => {
  const [data, setData] = useState([]);
  const [symbol, setSymbol] = useState('AAPL');

  useEffect(() => {
    axios.get(`https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=${symbol}&apikey=demo`)
     .then(response => {
        setData(response.data['Global Quote']);
      })
     .catch(error => {
        console.error(error);
      });
  }, [symbol]);

  const handleSymbolChange = (text) => {
    setSymbol(text);
  };

  return (
    <View style={styles.container}>
      <Text>Trading App</Text>
      <Text>Enter symbol:</Text>
      <TextInput
        style={styles.input}
        value={symbol}
        onChangeText={handleSymbolChange}
        placeholder="AAPL"
      />
      <Button title="Get Quote" onPress={() => axios.get(`https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=${symbol}&apikey=demo`)} />
      <Text>Quote:</Text>
      <Text>{data['05. price']}</Text>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
  input: {
    height: 40,
    borderColor: 'gray',
    borderWidth: 1,
    margin: 10,
    padding: 10,
  },
});

export default App;



// server.js
const express = require('express');
const app = express();
const mongoose = require('mongoose');
const jwt = require('jsonwebtoken');

mongoose.connect('mongodb://localhost/trading-app', { useNewUrlParser: true, useUnifiedTopology: true });

const userSchema = new mongoose.Schema({
  username: String,
  password: String,
});

const User = mongoose.model('User', userSchema);

app.use(express.json());

app.post('/login', (req, res) => {
  const { username, password } = req.body;
  User.findOne({ username, password }, (err, user) => {
    if (err) {
      res.status(500).send({ message: 'Error logging in' });
    } else if (!user) {
      res.status(401).send({ message: 'Invalid credentials' });
    } else {
      const token = jwt.sign({ userId: user._id }, 'ecretkey', { expiresIn: '1h' });
      res.send({ token });
    }
  });
});

app.get('/quote', authenticate, (req, res) => {
  const symbol = req.query.symbol;
  axios.get(`https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=${symbol}&apikey=demo`)
   .then(response => {
      res.send(response.data['Global Quote']);
    })
   .catch(error => {
      console.error(error);
      res.status(500).send({ message: 'Error fetching quote' });
    });
});

function authenticate(req, res, next) {
  const token = req.header('x-access-token');
  if (!token) {
    return res.status(401).send({ message: 'No token provided' });
  }
  jwt.verify(token, 'ecretkey', (err, decoded) => {
    if (err) {
      return res.status(401).send({ message: 'Invalid token' });
    }
    req.userId = decoded.userId;
    next();
  });
}

app.listen(3000, () => {
  console.log('Server started on port 3000');
});